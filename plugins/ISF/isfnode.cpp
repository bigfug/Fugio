#include "isfnode.h"

#include <cmath>

#include <QJsonDocument>
#include <QJsonArray>

#include <QByteArray>
#include <QPointF>
#include <QImage>
#include <QDateTime>

#if QT_VERSION >= QT_VERSION_CHECK( 5, 6, 0 )
#include <QOpenGLExtraFunctions>
#else
#warning Qt < 5.6 so ISF multipass render disabled
#endif

#include <fugio/core/uuid.h>
#include <fugio/colour/uuid.h>
#include <fugio/opengl/uuid.h>
#include <fugio/audio/uuid.h>

#include <fugio/context_signals.h>

#include <fugio/colour/colour_interface.h>
#include <fugio/choice_interface.h>

#include <fugio/opengl/texture_interface.h>

#include <fugio/audio/fft_interface.h>
#include <fugio/audio/audio_producer_interface.h>

#include <fugio/file/uuid.h>

#include <fugio/isf/uuid.h>

#include "isfplugin.h"

//#define OPENGL_DEBUG_ENABLE

#if defined( OPENGL_DEBUG_ENABLE )
#define OPENGL_PLUGIN_DEBUG 	checkErrors( __FILE__, __LINE__ );
//#define OPENGL_DEBUG(x) 	checkErrors( x, __FILE__, __LINE__ );
#else
#define OPENGL_PLUGIN_DEBUG
#define OPENGL_DEBUG(x)
#endif

#if defined( OPENGL_DEBUG_ENABLE )
void checkErrors( const char *file, int line )
{
#if !defined( GLU_VERSION )
	Q_UNUSED( file )
	Q_UNUSED( line )
#endif

	if( !ISFPlugin::hasContextStatic() )
	{
		return;
	}

#if defined( QT_OPENGL_ES )
	for( GLenum e = eglGetError() ; e != EGL_SUCCESS ; e = eglGetError() )
	{
		qDebug() << "EGL" << file << line << ":" << QString::number( e, 16 );
	}
#endif

	for( GLenum e = glGetError() ; e != GL_NO_ERROR ; e = glGetError() )
	{
#if defined( GLU_VERSION )
		qDebug() << "GL" << file << line << ":" << QString::fromLatin1( (const char *)gluErrorString( e ) );
#else
		qDebug() << "GL" << file << line << ":" << QString::number( e, 16 );
#endif
	}
}
#endif

ISFNode::ISFNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mVAO( 0 ),mProgram( 0 ), mFrameCounter( 0 ), mUniformTime( -1 ),
	  mTextureIndexCount( 0 ), mStartTime( -1 ), mLastRenderTime( 0 )
{
	FUGID( PIN_INPUT_SOURCE, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_FILENAME, "DFEBA477-4933-4C85-9B3B-24CE71053B1F" );
	FUGID( PIN_OUTPUT_RENDER, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );

	pinInput( "Trigger", PID_FUGIO_NODE_TRIGGER );

	mValInputFilename = pinInput<fugio::FilenameInterface *>( "Filename", mPinInputFilename, PID_FILENAME, PIN_INPUT_FILENAME );

	if( pNode->controlUuid() == NID_ISF )
	{
		mPinInputSource = pinInput( "Source", PIN_INPUT_SOURCE );
	}
	else
	{
		mPinInputFilename->setValue( ISFPlugin::instance()->pluginPath( pNode->controlUuid() ) );
	}

	mValOutputRender = pinOutput<fugio::RenderInterface *>( "Render", mPinOutputRender, PID_RENDER, PIN_OUTPUT_RENDER );
}

bool ISFNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	if( !ISFPlugin::hasContextStatic() )
	{
		return( false );
	}

	connect( mNode->context()->qobject(), SIGNAL(frameProcess(qint64)), this, SLOT(contextProcess(qint64)) );

	return( true );
}

bool ISFNode::deinitialise()
{
	disconnect( mNode->context()->qobject(), SIGNAL(frameProcess(qint64)), this, SLOT(contextProcess(qint64)) );

	if( ISFPlugin::hasContextStatic() )
	{
		mVAO.destroy();

		mBuffer.destroy();

		if( mProgram )
		{
			glDeleteProgram( mProgram );
		}

		QVector<GLuint>		TexLst;

		for( QMap<QString,ISFInput>::iterator it = mISFInputs.begin() ; it != mISFInputs.end() ; it++ )
		{
			ISFInput	&InpDat = it.value();

			if( InpDat.mTextureId )
			{
				TexLst << InpDat.mTextureId;
			}
		}

		for( QMap<QString,ISFImport>::iterator it = mISFImports.begin() ; it != mISFImports.end() ; it++ )
		{
			GLuint		TexId = it.value().mTextureId;

			if( TexId )
			{
				TexLst << TexId;
			}
		}

		for( ISFPass &Pass : mISFPasses )
		{
			if( Pass.mTextureId )
			{
				TexLst << Pass.mTextureId;
			}
		}

		if( !TexLst.isEmpty() )
		{
			glDeleteTextures( TexLst.size(), TexLst.constData() );
		}
	}

	for( QMap<QString,ISFInput>::iterator it = mISFInputs.begin() ; it != mISFInputs.end() ; it++ )
	{
		ISFInput	&InpDat = it.value();

		if( InpDat.mAudioInstance )
		{
			delete InpDat.mAudioInstance;

			InpDat.mAudioInstance = nullptr;
		}
	}

	mBuffer.destroy();

	mProgram = 0;

	return( NodeControlBase::deinitialise() );
}

void ISFNode::contextProcess( qint64 pTimeStamp )
{
	bool	AudioUpdate = false;

	for( QMap<QString,ISFInput>::iterator it = mISFInputs.begin() ; it != mISFInputs.end() ; it++ )
	{
		ISFInput	&InpDat = it.value();

		if( InpDat.mType != AUDIO )
		{
			continue;
		}

		QSharedPointer<fugio::PinInterface> P = mNode->findInputPinByName( it.key() );

		if( !P )
		{
			continue;
		}

		fugio::AudioProducerInterface	*AudInf = input<fugio::AudioProducerInterface *>( P );

		if( InpDat.mAudioInstance && ( !InpDat.mAudioInstance->isValid() || !AudInf ) )
		{
			delete InpDat.mAudioInstance;

			InpDat.mAudioInstance = nullptr;

			InpDat.mSamplePosition = 0;
		}

		if( !AudInf )
		{
			continue;
		}

		if( !InpDat.mAudioInstance )
		{
			if( InpDat.mTextureId )
			{
				glDeleteTextures( 1, &InpDat.mTextureId );

				InpDat.mTextureId = 0;
			}

			InpDat.mAudioInstance = AudInf->audioAllocInstance( 48000, fugio::AudioSampleFormat::Format32FS, 2 );

			InpDat.mAudioLatency = AudInf->audioLatency();
		}

		if( !InpDat.mAudioInstance )
		{
			continue;
		}

		const int			ChannelCount = InpDat.mAudioInstance->channels();
		const int			TotalSampleCount = ChannelCount * InpDat.mAudioMax;

		if( !InpDat.mTextureId )
		{
			glGenTextures( 1, &InpDat.mTextureId );

			if( InpDat.mTextureId )
			{
				OPENGL_PLUGIN_DEBUG;

				glBindTexture( GL_TEXTURE_2D, InpDat.mTextureId );

				glTexImage2D( GL_TEXTURE_2D, 0, GL_R32F, InpDat.mAudioMax, ChannelCount, 0, GL_RED, GL_FLOAT, nullptr );

				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ONE );

				glBindTexture( GL_TEXTURE_2D, 0 );

				OPENGL_PLUGIN_DEBUG;
			}
		}

		const qint64	CurPos = ( pTimeStamp * ( InpDat.mAudioInstance->sampleRate() / 1000 ) ) - InpDat.mAudioLatency;

		if( !InpDat.mSamplePosition )
		{
			InpDat.mSamplePosition = CurPos;
		}

//		if( InpDat.mSamplePosition < InpDat.mAudioInstance->producer()->audioAvailableMin() )
//		{
//			InpDat.mSamplePosition = CurPos;
//		}

		if( CurPos - InpDat.mSamplePosition >= InpDat.mAudioMax )
		{
			if( TotalSampleCount != InpDat.mAudioData.size() )
			{
				InpDat.mAudioData.resize( TotalSampleCount );
			}

			memset( InpDat.mAudioData.data(), 0, sizeof( float ) * InpDat.mAudioData.size() );

			QVector<float *>	AudPtr;

			AudPtr.resize( ChannelCount );

			for( int i = 0 ; i < ChannelCount ; i++ )
			{
				AudPtr[ i ] = InpDat.mAudioData.data() + ( InpDat.mAudioMax * i );
			}

			InpDat.mAudioInstance->audio( InpDat.mSamplePosition, InpDat.mAudioMax, 0, ChannelCount, (void **)AudPtr.data() );

			if( InpDat.mTextureId )
			{
				OPENGL_PLUGIN_DEBUG;

				glActiveTexture( GL_TEXTURE0 + InpDat.mTextureIndex );

				glBindTexture( GL_TEXTURE_2D, InpDat.mTextureId );

				glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, InpDat.mAudioMax, ChannelCount, GL_RED, GL_FLOAT, InpDat.mAudioData.constData() );

				glBindTexture( GL_TEXTURE_2D, 0 );

				glActiveTexture( GL_TEXTURE0 );

				OPENGL_PLUGIN_DEBUG;
			}

			InpDat.mSamplePosition += InpDat.mAudioMax;

			AudioUpdate = true;
		}
	}

	if( AudioUpdate )
	{
		pinUpdated( mPinOutputRender );
	}
}

void ISFNode::inputsUpdated( qint64 pTimeStamp )
{
	initializeOpenGLFunctions();

	if( pTimeStamp && mPinInputFilename->isUpdated( pTimeStamp ) )
	{
		QString		SrcPth = variant( mPinInputFilename ).toString();

		if( !SrcPth.isEmpty() )
		{
			QFile		SrcFil( SrcPth );

			if( !SrcFil.open( QFile::ReadOnly ) )
			{
				mNode->setStatus( NodeInterface::Error );
			}
			else
			{
				mNode->setStatus( NodeInterface::Initialised );

				parseISF( QFileInfo( SrcPth ).dir(), SrcFil.readAll() );
			}
		}
	}

	if( pTimeStamp && mPinInputSource && mPinInputSource->isUpdated( pTimeStamp ) )
	{
		QByteArray		Source = variant( mPinInputSource ).toByteArray();

		parseISF( QDir::current(), Source );
	}

	for( QMap<QString,ISFInput>::iterator it = mISFInputs.begin() ; it != mISFInputs.end() ; it++ )
	{
		ISFInput	&InpDat = it.value();

		QSharedPointer<fugio::PinInterface> P = mNode->findInputPinByName( it.key() );

		if( !P )
		{
			continue;
		}

		if( !P->isUpdated( pTimeStamp ) )
		{
			continue;
		}

		if( InpDat.mType == FFT )
		{
			QVector<float>			 FFT;

			FFT.resize( InpDat.mAudioMax );

			fugio::FftInterface		*FftInf = input<fugio::FftInterface *>( P );

			if( FftInf )
			{
				const int			sampleRate = FftInf->sampleRate();
				const int			timeSize   = FftInf->samples();

				const float				*DAT = FftInf->fft();

				if( !DAT )
				{
					return;
				}

				float		*ValOut = FFT.data();

				for( int i = 0 ; i < InpDat.mAudioMax ; i++ )
				{
					float	avg = 0;
					int		lowFreq = ( !i ? 0 : (int)((sampleRate/2) / powf( 2, InpDat.mAudioMax - i ) ) );
					int		hiFreq  = (int)((sampleRate/2) / powf( 2, InpDat.mAudioMax - 1 - i));
					int		lowBound = freqToIndex( timeSize, sampleRate, lowFreq );
					int		hiBound = freqToIndex( timeSize, sampleRate, hiFreq );

					for (int j = lowBound; j <= hiBound; j++)
					{
						const float	re = ( DAT[ ( i * 2 ) + 0 ] * 2.0f ) / float( timeSize );
						const float im = ( DAT[ ( i * 2 ) + 1 ] * 2.0f ) / float( timeSize );

						avg += sqrtf( re * re + im * im ) * 10.0f;
					}

					// line has been changed since discussion in the comments
					// avg /= (hiBound - lowBound);
					avg /= (hiBound - lowBound + 1);

					ValOut[ i ] = avg;
				}
			}


			if( !InpDat.mTextureId )
			{
				glGenTextures( 1, &InpDat.mTextureId );

				if( InpDat.mTextureId )
				{
					OPENGL_PLUGIN_DEBUG;

					glBindTexture( GL_TEXTURE_2D, InpDat.mTextureId );

					glTexImage2D( GL_TEXTURE_2D, 0, GL_R32F, InpDat.mAudioMax, 1, 0, GL_RED, GL_FLOAT, nullptr );

					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ONE );

					glBindTexture( GL_TEXTURE_2D, 0 );

					OPENGL_PLUGIN_DEBUG;
				}
			}

			if( InpDat.mTextureId )
			{
				OPENGL_PLUGIN_DEBUG;

				glActiveTexture( GL_TEXTURE0 + InpDat.mTextureIndex );

				glBindTexture( GL_TEXTURE_2D, InpDat.mTextureId );

				glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, FFT.size(), 1, GL_RED, GL_FLOAT, FFT.constData() );

				glBindTexture( GL_TEXTURE_2D, 0 );

				glActiveTexture( GL_TEXTURE0 );

				OPENGL_PLUGIN_DEBUG;
			}
		}
	}

	pinUpdated( mPinOutputRender );
}

ISFNode::ISFInputType ISFNode::isfType( QString Type )
{
	if( Type == "event" )		return( EVENT );
	if( Type == "bool" )		return( BOOL );
	if( Type == "long" )		return( LONG );
	if( Type == "float" )		return( FLOAT );
	if( Type == "point2D" )		return( POINT2D );
	if( Type == "image" )		return( IMAGE );
	if( Type == "color" )		return( COLOR );
	if( Type == "audio" )		return( AUDIO );
	if( Type == "audioFFT" )	return( FFT );

	return( UNKNOWN );
}

QMap<QString,ISFNode::ISFInput> ISFNode::parseInputs( QJsonArray Inputs )
{
	QMap<QString,ISFInput>	ISFNewInputs;

	for( const QJsonValue v : Inputs )
	{
		const QJsonObject	Input = v.toObject();
		QString				Name  = Input.value( "NAME" ).toString();
		ISFInputType		Type = isfType( Input.value( "TYPE" ).toString() );

		if( !Name.isEmpty() )
		{
			QSharedPointer<fugio::PinInterface>	ISFPin = mNode->findInputPinByName( Name );

			ISFInput							CurISF = mISFInputs.value( Name );

			if( ISFPin && CurISF.mType != UNKNOWN && CurISF.mType != Type )
			{
				mNode->removePin( ISFPin );
			}

			CurISF.mType = Type;

			const bool	NewPin = ISFPin ? false : true;

			if( !ISFPin )
			{
				ISFPin = pinInput( Name, QUuid::createUuid() );
			}

			if( !ISFPin )
			{
				continue;
			}

			switch( Type )
			{
				case UNKNOWN:
				case EVENT:
					break;

				case BOOL:
					if( NewPin )
					{
						ISFPin->setValue( Input.value( "DEFAULT" ).toBool() );
					}
					break;

				case LONG:
					{
						QJsonArray	Labels = Input.value( "LABELS" ).toArray();
						QJsonArray	Values = Input.value( "VALUES" ).toArray();

						QStringList		LabLst;
						QVariantList	ValLst;

						for( QJsonValueRef L : Labels )
						{
							LabLst << L.toString();
						}

						for( QJsonValueRef V : Values )
						{
							ValLst << V.toInt();
						}

						QSharedPointer<fugio::PinControlInterface>	PinInf = ISFPin->control();

						if( PinInf && ISFPin->controlUuid() != PID_CHOICE )
						{
							ISFPin->setControl( QSharedPointer<fugio::PinControlInterface>() );

							PinInf.clear();
						}

						if( !PinInf )
						{
							PinInf = mNode->context()->createPinControl( PID_CHOICE, ISFPin );

							if( PinInf )
							{
								ISFPin->setControl( PinInf );
							}
						}

						if( PinInf )
						{
							fugio::ChoiceInterface	*ChoiceInf = qobject_cast<fugio::ChoiceInterface *>( PinInf->qobject() );

							if( ChoiceInf )
							{
								ChoiceInf->setChoices( LabLst );

								ISFPin->setSetting( "VALUES", ValLst );
							}
						}

						if( NewPin )
						{
							int		DefVal = Input.value( "DEFAULT" ).toInt();

							if( DefVal < LabLst.size() )
							{
								ISFPin->setValue( LabLst.at( DefVal ) );
							}
						}
					}
					break;

				case FLOAT:
					if( NewPin )
					{
						ISFPin->setValue( Input.value( "DEFAULT" ).toDouble() );
					}
					break;

				case POINT2D:
					ISFPin->registerPinInputType( PID_POINT );

					if( NewPin )
					{
						ISFPin->setValue( QPointF() );
					}
					break;

				case IMAGE:
					ISFPin->registerPinInputType( PID_OPENGL_TEXTURE );
					break;

				case COLOR:
					{
						ISFPin->registerPinInputType( PID_COLOUR );

						QColor		C;

						if( NewPin )
						{
							QJsonArray	DefaultColour = Input.value( "DEFAULT" ).toArray();

							if( !DefaultColour.isEmpty() )
							{
								if( DefaultColour.size() > 0 ) C.setRedF( DefaultColour.at( 0 ).toDouble() );
								if( DefaultColour.size() > 1 ) C.setGreenF( DefaultColour.at( 1 ).toDouble() );
								if( DefaultColour.size() > 2 ) C.setBlueF( DefaultColour.at( 2 ).toDouble() );
								if( DefaultColour.size() > 3 ) C.setAlphaF( DefaultColour.at( 3 ).toDouble() );
							}

							ISFPin->setValue( C );
						}
					}
					break;

				case AUDIO:
					ISFPin->registerPinInputType( PID_AUDIO );

					CurISF.mAudioMax = Input.value( "MAX" ).toInt( 256 );
					break;

				case FFT:
					ISFPin->registerPinInputType( PID_FFT );

					CurISF.mAudioMax = Input.value( "MAX" ).toInt( 256 );
					break;
			}

			ISFNewInputs[ Name ] = CurISF;
		}
	}

	return( ISFNewInputs );
}

QMap<QString, ISFNode::ISFImport> ISFNode::parseImports( const QDir &pDir, const QJsonObject Imports ) const
{
	QMap<QString,ISFImport>		NewImports;

	for( const QString Name : Imports.keys() )
	{
		QString				Path  = Imports.value( Name ).toObject().value( "PATH" ).toString();

		Path = pDir.absoluteFilePath( Path );

		if( mISFImports.contains( Name ) )
		{
			if( mISFImports.value( Name ).mPath == Path )
			{
				continue;
			}

			NewImports.insert( Name, mISFImports.value( Name ) );
		}

		ISFImport	Import;

		Import.mPath = Path;

		QImage		ImportImage( Path );

		if( ImportImage.isNull() )
		{
			qWarning() << "ISF cannot load IMPORT" << Path;

			continue;
		}

		NewImports.insert( Name, Import );
	}

	return( NewImports );
}

QList<ISFNode::ISFPass> ISFNode::parsePasses( const QJsonArray pPasses ) const
{
	QList<ISFPass>		PassList;

	for( const QJsonValue v : pPasses )
	{
		const QJsonObject	PassObject = v.toObject();

		ISFPass				Pass;

		Pass.mTarget = PassObject.value( "TARGET" ).toString();
		Pass.mPersistent = PassObject.value( "PERSISTENT" ).toBool();
		Pass.mFloat = PassObject.value( "FLOAT" ).toBool();
		Pass.mWidth = PassObject.value( "WIDTH" ).toString();
		Pass.mHeight = PassObject.value( "HEIGHT" ).toString();

		PassList << Pass;
	}

	if( PassList.isEmpty() )
	{
		PassList << ISFPass();
	}

	return( PassList );
}

void ISFNode::parseISF( const QDir &pDir, const QByteArray pSource )
{
	int			CommentStart = pSource.indexOf( "/*" );
	int			CommentEnd   = pSource.indexOf( "*/", CommentStart );

	if( CommentStart == -1 || CommentEnd == -1 )
	{
		return;
	}

	QByteArray		Comment = pSource.mid( CommentStart + 2, CommentEnd - CommentStart - 2 );

	QJsonParseError	JERR;

	QJsonDocument	JSON = QJsonDocument::fromJson( Comment, &JERR );

	if( JSON.isNull() )
	{
		qWarning() << JERR.errorString();

		return;
	}

	const QJsonObject	JOBJ = JSON.object();

	//	if( !JOBJ.contains( "ISFVSN" ) || JOBJ.value( "ISFVSN" ).toString().split( '.' ).first() != "2" )
	//	{
	//		return;
	//	}

	// Process INPUTS

	QStringList				OldInputs = mISFInputs.keys();

	QJsonArray				Inputs = JOBJ.value( "INPUTS" ).toArray();

	QMap<QString,ISFInput>	NewInputs = parseInputs( Inputs );

	for( const QString &Name : NewInputs.keys() )
	{
		OldInputs.removeOne( Name );
	}

	for( QString Name : OldInputs )
	{
		QSharedPointer<fugio::PinInterface>	ISFPin = mNode->findInputPinByName( Name );

		if( ISFPin )
		{
			mNode->removePin( ISFPin );
		}

		ISFInput	&InpDat = mISFInputs[ Name ];

		if( InpDat.mAudioInstance )
		{
			delete InpDat.mAudioInstance;

			InpDat.mAudioInstance = nullptr;
		}
	}

	mISFInputs = NewInputs;

	// Process IMPORTED

	QJsonObject				Imports = JOBJ.value( "IMPORTED" ).toObject();

	QMap<QString,ISFImport>	NewImports = parseImports( pDir, Imports );

	QVector<GLuint>		TexLst;

	for( const QString &Name : mISFImports.keys() )
	{
		GLuint		OldTexId = mISFImports.value( Name ).mTextureId;

		if( NewImports.contains( Name ) && NewImports.value( Name ).mTextureId == OldTexId )
		{
			continue;
		}

		if( OldTexId )
		{
			TexLst << OldTexId;
		}
	}

	// Process PASSES

	QJsonArray				PassesArray = JOBJ.value( "PASSES" ).toArray();

	for( ISFPass &Pass : mISFPasses )
	{
		if( Pass.mTextureId )
		{
			TexLst << Pass.mTextureId;

			Pass.mTextureId = 0;
		}
	}

	mISFPasses = parsePasses( PassesArray );

	// Delete all unused textures

	if( !TexLst.isEmpty() )
	{
		glDeleteTextures( TexLst.size(), TexLst.constData() );
	}

	// Load the shader

	QString		ShaderSource = pSource.mid( CommentEnd + 2 );

	if( !loadShaders( ShaderSource ) )
	{
		return;
	}

	// Find all the uniform locations in the shader for our inputs and imports

	GLint		TextureIndex = 0;

	for( QMap<QString,ISFInput>::iterator it = mISFInputs.begin() ; it != mISFInputs.end() ; it++ )
	{
		ISFInput	&Inp = it.value();

		Inp.mUniform = glGetUniformLocation( mProgram, it.key().toLatin1() );

		if( Inp.mType == IMAGE || Inp.mType == AUDIO || Inp.mType == FFT )
		{
			Inp.mTextureIndex = TextureIndex++;
		}
	}

	for( QMap<QString,ISFImport>::iterator it = mISFImports.begin() ; it != mISFImports.end() ; it++ )
	{
		ISFImport	&Imp = it.value();

		Imp.mUniform = glGetUniformLocation( mProgram, it.key().toLatin1() );

		Imp.mTextureIndex = TextureIndex++;
	}

	for( ISFPass &Pass : mISFPasses )
	{
		if( !Pass.mTarget.isEmpty() )
		{
			Pass.mUniform = glGetUniformLocation( mProgram, Pass.mTarget.toLatin1() );

			Pass.mTextureIndex = TextureIndex++;
		}
	}

	mTextureIndexCount = TextureIndex;
}

bool ISFNode::loadShaders( const QString &pShaderSource )
{
	GLint	Result = GL_FALSE;
	GLint	InfoLogLength = 0;

	GLuint	VertexShader = glCreateShader( GL_VERTEX_SHADER );

	const QByteArray		VS =
			"#version 330 core\n"
			"#define vv_vertShaderInit isf_vertShaderInit\n"
			"#define vv_FragNormCoord isf_FragNormCoord\n"
			"layout( location = 0 ) in vec2 vertex;\n"
			"out vec2 isf_FragNormCoord;\n"
			"void isf_vertShaderInit( void )\n"
			"{\n"
			"   gl_Position = vec4( vertex, 0, 1 );\n"
			"	isf_FragNormCoord = ( vertex * 0.5 ) + 0.5;\n"
			"}\n"
			"\n"
			"void main(void)\n"
			"{\n"
			"	isf_vertShaderInit();"
			"}\n";

	const char			*VertexSourceArray = VS.constData();

	glShaderSource( VertexShader, 1, &VertexSourceArray, NULL );

	glCompileShader( VertexShader );

	glGetShaderiv( VertexShader, GL_COMPILE_STATUS, &Result );
	glGetShaderiv( VertexShader, GL_INFO_LOG_LENGTH, &InfoLogLength );

	if( Result == GL_FALSE )
	{
		QByteArray		InfoLogMessage;

		InfoLogMessage.resize( InfoLogLength );

		glGetShaderInfoLog( VertexShader, InfoLogLength, NULL, InfoLogMessage.data() );

		qWarning() << "ISF Vertex Shader:" << QString::fromLatin1( InfoLogMessage );

		return( false );
	}

	GLuint		FragmentShader = glCreateShader( GL_FRAGMENT_SHADER );

	QByteArray		FragmentSource =
			"#version 330 core\n"
			"#define vv_FragNormCoord isf_FragNormCoord\n"
			"#define gl_FragColor isf_OutputColour\n"
			"#define IMG_SIZE(i) textureSize(i,0)\n"
			"#define IMG_PIXEL(i,p) texelFetch(i,ivec2(p),0)\n"
			"#define IMG_NORM_PIXEL(i,p) texture(i,p)\n"
			"#define IMG_THIS_PIXEL(i) IMG_PIXEL(i,gl_FragCoord.xy)\n"
			"#define IMG_THIS_NORM_PIXEL(i) IMG_NORM_PIXEL(i,isf_FragNormCoord)\n"
			"uniform int PASSINDEX;\n"
			"uniform vec2 RENDERSIZE;\n"
			"uniform float TIME;\n"
			"uniform float TIMEDELTA;\n"
			"uniform vec4 DATE;\n"
			"uniform int FRAMEINDEX;\n"
			"in vec2 isf_FragNormCoord;\n"
			"out vec4 isf_OutputColour;\n";

	for( QMap<QString,ISFInput>::iterator it = mISFInputs.begin() ; it != mISFInputs.end() ; it++ )
	{
		switch( it.value().mType )
		{
			case EVENT:
			case BOOL:
				FragmentSource.append( QString( "uniform bool %1;\n" ).arg( it.key() ) );
				break;

			case LONG:
				FragmentSource.append( QString( "uniform int %1;\n" ).arg( it.key() ) );
				break;

			case FLOAT:
				FragmentSource.append( QString( "uniform float %1;\n" ).arg( it.key() ) );
				break;

			case POINT2D:
				FragmentSource.append( QString( "uniform vec2 %1;\n" ).arg( it.key() ) );
				break;

			case COLOR:
				FragmentSource.append( QString( "uniform vec4 %1;\n" ).arg( it.key() ) );
				break;

			case IMAGE:
			case AUDIO:
			case FFT:
				FragmentSource.append( QString( "uniform sampler2D %1;\n" ).arg( it.key() ) );
				break;

			case UNKNOWN:
				break;
		}
	}

	for( QMap<QString,ISFImport>::iterator it = mISFImports.begin() ; it != mISFImports.end() ; it++ )
	{
		FragmentSource.append( QString( "uniform sampler2D %1;\n" ).arg( it.key() ) );
	}

	for( ISFPass &PassData : mISFPasses )
	{
		if( !PassData.mTarget.isEmpty() )
		{
			FragmentSource.append( QString( "uniform sampler2D %1;\n" ).arg( PassData.mTarget ) );
		}
	}

	FragmentSource.append( pShaderSource );

	//	qDebug() << FragmentSource.split( '\n' );

	const char			*FragmentSourceArray = FragmentSource.constData();

	glShaderSource( FragmentShader, 1, &FragmentSourceArray, NULL );

	glCompileShader( FragmentShader );

	glGetShaderiv( FragmentShader, GL_COMPILE_STATUS, &Result );
	glGetShaderiv( FragmentShader, GL_INFO_LOG_LENGTH, &InfoLogLength );

	if( Result == GL_FALSE )
	{
		QByteArray		InfoLogMessage;

		InfoLogMessage.resize( InfoLogLength );

		glGetShaderInfoLog( FragmentShader, InfoLogLength, NULL, InfoLogMessage.data() );

		qWarning() << "Fragment Shader:" << QString::fromLatin1( InfoLogMessage );

		glDeleteShader( VertexShader );

		return( false );
	}

	GLuint		Program = glCreateProgram();

	glAttachShader( Program, VertexShader );
	glAttachShader( Program, FragmentShader );

	glLinkProgram( Program );

	glGetProgramiv( Program, GL_LINK_STATUS, &Result );
	glGetProgramiv( Program, GL_INFO_LOG_LENGTH, &InfoLogLength );

	if( Result == GL_FALSE )
	{
		QByteArray		InfoLogMessage;

		InfoLogMessage.resize( InfoLogLength );

		glGetProgramInfoLog( Program, InfoLogLength, NULL, InfoLogMessage.data() );

		qWarning() << "Shader Link:" << QString::fromLatin1( InfoLogMessage );

		glDeleteShader( VertexShader );
		glDeleteShader( FragmentShader );

		return( false );
	}

	if( mProgram )
	{
		glDeleteProgram( mProgram );
	}

	mProgram = Program;

	mFrameCounter = 0;

	mStartTime = -1;

	mUniformTime = glGetUniformLocation( mProgram, "TIME" );
	mUniformDate = glGetUniformLocation( mProgram, "DATE" );
	mUniformRenderSize = glGetUniformLocation( mProgram, "RENDERSIZE" );
	mUniformPassIndex = glGetUniformLocation( mProgram, "PASSINDEX" );
	mUniformTimeDelta = glGetUniformLocation( mProgram, "TIMEDELTA" );
	mUniformFrameIndex = glGetUniformLocation( mProgram, "FRAMEINDEX" );

	return( true );
}

float ISFNode::getBandWidth(float timeSize, float sampleRate)
{
	return( ( 2.0f / timeSize ) * ( sampleRate / 2.0f ) );
}

int ISFNode::freqToIndex(int timeSize, int sampleRate, int freq)
{
	// special case: freq is lower than the bandwidth of spectrum[0]
	if ( freq < getBandWidth( timeSize, sampleRate )/2 ) return 0;
	// special case: freq is within the bandwidth of spectrum[512]
	if ( freq > sampleRate/2 - getBandWidth( timeSize, sampleRate )/2 ) return timeSize/2;
	// all other cases
	float fraction = (float)freq/(float) sampleRate;
	int i = qRound(timeSize * fraction);
	return i;
}

void ISFNode::renderInputs()
{
	for( QMap<QString,ISFInput>::iterator it = mISFInputs.begin() ; it != mISFInputs.end() ; it++ )
	{
		ISFInput		&InpDat = it.value();
		const GLint		 UniLoc = InpDat.mUniform;

		if( UniLoc == -1 )
		{
			continue;
		}

		QSharedPointer<fugio::PinInterface> P = mNode->findInputPinByName( it.key() );

		if( !P )
		{
			continue;
		}

		if( InpDat.mType == EVENT && InpDat.mEventFlag )
		{
			glUniform1i( UniLoc, false );

			InpDat.mEventFlag = false;
		}

		switch( InpDat.mType )
		{
			case UNKNOWN:
				break;

			case EVENT:
				glUniform1i( UniLoc, true );

				InpDat.mEventFlag = true;
				break;

			case BOOL:
				glUniform1i( UniLoc, variant( P ).toBool() );
				break;

			case LONG:
				glUniform1i( UniLoc, variant( P ).toInt() );
				break;

			case FLOAT:
				glUniform1f( UniLoc, variant( P ).toFloat() );
				break;

			case POINT2D:
				{
					QPointF	V = variant( P ).toPointF();

					glUniform2f( UniLoc, V.x(), V.y() );
				}
				break;

			case COLOR:
				{
					QColor	C = variant( P ).value<QColor>();

					glUniform4f( UniLoc, C.redF(), C.greenF(), C.blueF(), C.alphaF() );
				}
				break;

			case IMAGE:
				{
					fugio::OpenGLTextureInterface	*TexInf = input<fugio::OpenGLTextureInterface *>( P );

					if( TexInf )
					{
						glUniform1i( UniLoc, InpDat.mTextureIndex );

						glActiveTexture( GL_TEXTURE0 + InpDat.mTextureIndex );

						TexInf->srcBind();
					}
				}
				break;

			case AUDIO:
			case FFT:
				if( InpDat.mTextureId )
				{
					glUniform1i( UniLoc, InpDat.mTextureIndex );

					glActiveTexture( GL_TEXTURE0 + InpDat.mTextureIndex );

					glBindTexture( GL_TEXTURE_2D, InpDat.mTextureId );
				}
				break;
		}
	}

	glActiveTexture( GL_TEXTURE0 );
}

void ISFNode::renderImports()
{
	for( QMap<QString,ISFImport>::iterator it = mISFImports.begin() ; it != mISFImports.end() ; it++ )
	{
		ISFImport		&ImpDat = it.value();

		if( ImpDat.mUniform == -1 )
		{
			continue;
		}

		// Avoid disturbing other texture bindings

		glActiveTexture( GL_TEXTURE0 + ImpDat.mTextureIndex );

		if( !ImpDat.mTextureId )
		{
			QImage		ImportImage( ImpDat.mPath );

			if( ImportImage.isNull() )
			{
				continue;
			}

			glGenTextures( 1, &ImpDat.mTextureId );

			if( ImpDat.mTextureId )
			{
				glBindTexture( GL_TEXTURE_2D, ImpDat.mTextureId );

				switch( ImportImage.format() )
				{
					case QImage::Format_ARGB32:
#if defined( GL_BGRA )
						glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, ImportImage.width(), ImportImage.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, ImportImage.constBits() );
#else
						glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, ImportImage.width(), ImportImage.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, ImportImage.constBits() );
#endif
						break;

					case QImage::Format_RGB888:
#if defined( GL_BGR )
						glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, ImportImage.width(), ImportImage.height(), 0, GL_BGR, GL_UNSIGNED_BYTE, ImportImage.constBits() );
#else
						glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, ImportImage.width(), ImportImage.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, ImportImage.constBits() );
#endif
						break;

					case QImage::Format_RGBA8888:
#if defined( GL_BGRA )
						glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, ImportImage.width(), ImportImage.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, ImportImage.constBits() );
#else
						glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, ImportImage.width(), ImportImage.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, ImportImage.constBits() );
#endif
						break;

					default:
#if defined( GL_BGRA )
						glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, ImportImage.width(), ImportImage.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, nullptr );
#else
						glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, ImportImage.width(), ImportImage.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr );
#endif
						break;
				}

				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

				glBindTexture( GL_TEXTURE_2D, 0 );
			}
		}

		glBindTexture( GL_TEXTURE_2D, ImpDat.mTextureId );
	}

	glActiveTexture( GL_TEXTURE0 );
}

void ISFNode::renderPasses( GLint Viewport[ 4 ] )
{
#if defined( QOPENGLEXTRAFUNCTIONS_H )
	QOpenGLExtraFunctions	*GLEX = QOpenGLContext::currentContext()->extraFunctions();
#endif

	for( ISFPass &PassData : mISFPasses )
	{
		if( PassData.mTarget.isEmpty() )
		{
			continue;
		}

		if( !PassData.mFBO )
		{
			glGenFramebuffers( 1, &PassData.mFBO );
		}

		if( !PassData.mFBO )
		{
			continue;
		}

		QSize		NewSze( Viewport[ 2 ], Viewport[ 3 ] );

		NewSze.setWidth( calculateValue( NewSze.width(), NewSze, PassData.mWidth ) );
		NewSze.setHeight( calculateValue( NewSze.height(), NewSze, PassData.mHeight ) );

		if( NewSze != PassData.mSize )
		{
			if( PassData.mTextureId )
			{
				glBindFramebuffer( GL_FRAMEBUFFER, PassData.mFBO );

				glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0 );

				glBindFramebuffer( GL_FRAMEBUFFER, 0 );

				glDeleteTextures( 1, &PassData.mTextureId );

				PassData.mTextureId = 0;
			}

			PassData.mSize = NewSze;
		}

		if( PassData.mTextureId )
		{
			continue;
		}

		// Avoid disturbing other texture bindings

		glActiveTexture( GL_TEXTURE0 + PassData.mTextureIndex );

		// Allocate a new texture

		glGenTextures( 1, &PassData.mTextureId );

		if( PassData.mTextureId )
		{
			glBindTexture( GL_TEXTURE_2D, PassData.mTextureId );

			glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, PassData.mSize.width(), PassData.mSize.height(), 0, GL_RGBA, PassData.mFloat ? GL_FLOAT : GL_UNSIGNED_BYTE, NULL );

			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

			glBindTexture( GL_TEXTURE_2D, 0 );
		}

		glActiveTexture( GL_TEXTURE0 );

		if( !PassData.mTextureId )
		{
			continue;
		}

		// Set up our FBO

		glBindFramebuffer( GL_FRAMEBUFFER, PassData.mFBO );

		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, PassData.mTextureId, 0 );

#if defined( QOPENGLEXTRAFUNCTIONS_H )
		GLenum DrawBuffers[ 1 ] = { GL_COLOR_ATTACHMENT0 };

		if( GLEX )
		{
			GLEX->glDrawBuffers( 1, DrawBuffers );
		}
#endif

		if( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
		{
			qWarning() << "ISF GL_FRAMEBUFFER != GL_FRAMEBUFFER_COMPLETE";
		}

		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	}
}

void ISFNode::renderUniforms( qint64 pTimeStamp, GLint Viewport[ 4 ] )
{
	if( mUniformRenderSize != -1 )
	{
		glUniform2f( mUniformRenderSize, Viewport[ 2 ], Viewport[ 3 ] );
	}

	if( mUniformTime != -1 )
	{
		glUniform1f( mUniformTime, GLfloat( pTimeStamp - mStartTime ) / 1000.0f );
	}

	if( mUniformTimeDelta != -1 )
	{
		glUniform1f( mUniformTimeDelta, GLfloat( pTimeStamp - mLastRenderTime ) / 1000.0f );
	}

	if( mUniformFrameIndex != -1 )
	{
		glUniform1i( mUniformFrameIndex, mFrameCounter );
	}

	if( mUniformDate != -1 )
	{
		const QDateTime		DateTime = QDateTime::currentDateTime();

		glUniform4f( mUniformDate, DateTime.date().year(), DateTime.date().month(), DateTime.date().day(), DateTime.time().msecsSinceStartOfDay() / 1000 );
	}
}

int ISFNode::calculateValue( int pValue, const QSize &pSize, QString pExpression ) const
{
	if( pExpression.isEmpty() )
	{
		return( pValue );
	}

#if defined( INCLUDE_EXPRTK_HPP )
	typedef qreal						        exprtk_type;
	typedef exprtk::symbol_table<exprtk_type>   symbol_table_t;
	typedef exprtk::expression<exprtk_type>		expression_t;
	typedef exprtk::parser<exprtk_type>		    parser_t;

	QMap<std::string,exprtk_type>				VarMap;

	VarMap.insert( "isf_WIDTH", pSize.width() );
	VarMap.insert( "isf_HEIGHT", pSize.height() );

	pExpression = pExpression.replace( QRegExp( "\\$WIDTH\\b" ), "isf_WIDTH" );
	pExpression = pExpression.replace( QRegExp( "\\$HEIGHT\\b" ), "isf_HEIGHT" );

	for( QMap<QString,ISFInput>::const_iterator it = mISFInputs.begin() ; it != mISFInputs.end() ; it++ )
	{
		const ISFInput		&InpDat = it.value();

		pExpression = pExpression.replace( QRegExp( QString( "\\$%1\\b" ).arg( it.key() ) ), QString( "isf_%1" ).arg( it.key() ) );

		QSharedPointer<fugio::PinInterface> P = mNode->findInputPinByName( it.key() );

		if( !P )
		{
			continue;
		}

		switch( InpDat.mType )
		{
			case BOOL:
				VarMap.insert( QString( it.key() ).prepend( "isf_" ).toStdString(), variant( P ).toBool() );
				break;

			case LONG:
				VarMap.insert( QString( it.key() ).prepend( "isf_" ).toStdString(), variant( P ).toInt() );
				break;

			case FLOAT:
				VarMap.insert( QString( it.key() ).prepend( "isf_" ).toStdString(), variant( P ).toFloat() );
				break;

			default:
				break;
		}
	}

	symbol_table_t symbol_table;

	for( QMap<std::string,exprtk_type>::iterator it = VarMap.begin() ; it != VarMap.end() ; it++ )
	{
		symbol_table.add_variable( it.key(), it.value(), true );
	}

	expression_t expression;

	expression.register_symbol_table( symbol_table );

	parser_t parser;

	if( parser.compile( pExpression.toStdString(), expression ) )
	{
		pValue = expression.value();
	}
#endif

	return( pValue );
}

void ISFNode::render( qint64 pTimeStamp, QUuid pSourcePinId )
{
	Q_UNUSED( pSourcePinId )

	if( !ISFPlugin::hasContextStatic() )
	{
		return;
	}

	initializeOpenGLFunctions();

	GLint		Viewport[ 4 ];

	glGetIntegerv( GL_VIEWPORT, Viewport );

	GLint		FBOCur;

	glGetIntegerv( GL_FRAMEBUFFER_BINDING, &FBOCur );

	GLint		VAOCur;

	glGetIntegerv( GL_VERTEX_ARRAY_BINDING, &VAOCur );

	GLint		PRGCur;

	glGetIntegerv( GL_CURRENT_PROGRAM, &PRGCur );

	if( mStartTime == -1 )
	{
		mStartTime = pTimeStamp;
	}

	QOpenGLVertexArrayObject::Binder VAOBinder( &mVAO );

	GLfloat		Verticies[][ 2 ] =
	{
		{ -1, -1 },
		{ -1,  1 },
		{  1, -1 },
		{  1,  1 }
	};

	mBuffer.create();
	mBuffer.bind();
	mBuffer.allocate( Verticies, sizeof( Verticies ) );

	if( mProgram )
	{
		glClearColor( 0.0f, 0.0f, 0.4f, 0.0f );

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		glEnableVertexAttribArray( 0 );

		glVertexAttribPointer(
					0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
					2,                  // size
					GL_FLOAT,           // type
					GL_FALSE,           // normalized?
					0,                  // stride
					(void*)0            // array buffer offset
					);

		glUseProgram( mProgram );

		//---------------------------------------------------------------------
		// Set the INPUTS uniforms

		renderInputs();

		//---------------------------------------------------------------------
		// Set up the IMPORTED textures

		renderImports();

		//---------------------------------------------------------------------
		// Set predefined uniforms

		renderUniforms( pTimeStamp, Viewport );

		//---------------------------------------------------------------------
		// Updated passes

		renderPasses( Viewport );

		OPENGL_PLUGIN_DEBUG;

		//---------------------------------------------------------------------
		// Bind PASSES render targets

		for( ISFPass &PassData : mISFPasses )
		{
			if( PassData.mTextureIndex != -1 && PassData.mUniform != -1 )
			{
				glUniform1i( PassData.mUniform, PassData.mTextureIndex );

				glActiveTexture( GL_TEXTURE0 + PassData.mTextureIndex );

				glBindTexture( GL_TEXTURE_2D, PassData.mTextureId );
			}
		}

		OPENGL_PLUGIN_DEBUG;

		//---------------------------------------------------------------------
		// Perform rendering passes

		for( int Pass = 0 ; Pass < mISFPasses.size() ; Pass++ )
		{
			if( mUniformPassIndex != -1 )
			{
				glUniform1i( mUniformPassIndex, Pass );
			}

			ISFPass		&PassData = mISFPasses[ Pass ];

			// we can't read and write to textures at the same time

			if( PassData.mTextureId )
			{
				glActiveTexture( GL_TEXTURE0 + PassData.mTextureIndex );

				glBindTexture( GL_TEXTURE_2D, 0 );
			}

			if( PassData.mFBO )
			{
				glBindFramebuffer( GL_FRAMEBUFFER, PassData.mFBO );

				glViewport( 0, 0, PassData.mSize.width(), PassData.mSize.height() );
			}

			glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

			if( PassData.mFBO )
			{
				glBindFramebuffer( GL_FRAMEBUFFER, FBOCur );

				glViewport( Viewport[ 0 ], Viewport[ 1 ], Viewport[ 2 ], Viewport[ 3 ] );
			}

			if( PassData.mTextureId )
			{
				glActiveTexture( GL_TEXTURE0 + PassData.mTextureIndex );

				glBindTexture( GL_TEXTURE_2D, PassData.mTextureId );
			}
		}

		//---------------------------------------------------------------------
		// Clean-up

		glDisableVertexAttribArray( 0 );

		glUseProgram( 0 );

		// release all textures

		for( GLuint i = 0 ; i < mTextureIndexCount ; i++ )
		{
			glActiveTexture( GL_TEXTURE0 + i );

			glBindTexture( GL_TEXTURE_2D, 0 );
		}

		glActiveTexture( GL_TEXTURE0 );

		glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

		if( mLastRenderTime != pTimeStamp )
		{
			mLastRenderTime = pTimeStamp;

			mFrameCounter++;
		}
	}
}
