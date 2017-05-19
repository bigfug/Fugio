#include "isfnode.h"

#include <QJsonDocument>
#include <QJsonArray>

#include <QByteArray>
#include <QPointF>

#include <fugio/core/uuid.h>
#include <fugio/colour/uuid.h>
#include <fugio/opengl/uuid.h>
#include <fugio/audio/uuid.h>

#include <fugio/colour/colour_interface.h>
#include <fugio/choice_interface.h>

#include <fugio/opengl/texture_interface.h>

#include "isfplugin.h"

ISFNode::ISFNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mVAO( 0 ), mBuffer( 0 ), mProgram( 0 ), mLastRenderTime( 0 )
{
	FUGID( PIN_INPUT_SOURCE, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_OUTPUT_RENDER, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );

	mPinInputSource = pinInput( "Source", PIN_INPUT_SOURCE );

	mValOutputRender = pinOutput<fugio::RenderInterface *>( "Render", mPinOutputRender, PID_RENDER, PIN_OUTPUT_RENDER );
}

bool ISFNode::deinitialise()
{
	if( ISFPlugin::hasContextStatic() )
	{
		if( mVAO )
		{
			glDeleteVertexArrays( 1, &mVAO );
		}

		if( mBuffer )
		{
			glDeleteBuffers( 1, &mBuffer );
		}

		if( mProgram )
		{
			glDeleteProgram( mProgram );
		}
	}

	mVAO = 0;

	mBuffer = 0;

	mProgram = 0;

	return( NodeControlBase::deinitialise() );
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
	if( Type == "audioFFT" )	return( AUDIOFFT );

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
					ISFPin->setValue( Input.value( "DEFAULT" ).toBool() );
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

						int		DefVal = Input.value( "DEFAULT" ).toInt();

						if( DefVal < LabLst.size() )
						{
							ISFPin->setValue( LabLst.at( DefVal ) );
						}
					}
					break;

				case FLOAT:
					ISFPin->setValue( Input.value( "DEFAULT" ).toDouble() );
					break;

				case POINT2D:
					ISFPin->registerPinInputType( PID_POINT );

					ISFPin->setValue( QPointF() );
					break;

				case IMAGE:
					ISFPin->registerPinInputType( PID_OPENGL_TEXTURE );
					break;

				case COLOR:
					{
						ISFPin->registerPinInputType( PID_COLOUR );

						QColor		C;

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
					break;

				case AUDIO:
					ISFPin->registerPinInputType( PID_AUDIO );
					break;

				case AUDIOFFT:
					ISFPin->registerPinInputType( PID_FFT );
					break;
			}

			ISFNewInputs[ Name ] = CurISF;
		}
	}

	return( ISFNewInputs );
}

void ISFNode::inputsUpdated( qint64 pTimeStamp )
{
	if( pTimeStamp && mPinInputSource->isUpdated( pTimeStamp ) )
	{
		QByteArray		Source = variant( mPinInputSource ).toByteArray();

		int			CommentStart = Source.indexOf( "/*" );
		int			CommentEnd   = Source.indexOf( "*/", CommentStart );

		if( CommentStart != -1 && CommentEnd != -1 )
		{
			QByteArray		Comment = Source.mid( CommentStart + 2, CommentEnd - CommentStart - 2 );

			QJsonParseError	JERR;

			QJsonDocument	JSON = QJsonDocument::fromJson( Comment, &JERR );

			if( !JSON.isNull() )
			{
				const QJsonObject	JOBJ = JSON.object();

				if( JOBJ.contains( "ISFVSN" ) && JOBJ.value( "ISFVSN" ).toString().split( '.' ).first() == "2" )
				{
					mShaderSource = Source.mid( CommentEnd + 2 );

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
					}

					mISFInputs = NewInputs;

					loadShaders();
				}
			}
			else
			{
				qWarning() << JERR.errorString();
			}
		}
	}

	pinUpdated( mPinOutputRender );
}

void ISFNode::loadShaders()
{
	GLint	Result = GL_FALSE;
	GLint	InfoLogLength = 0;

	GLuint	VertexShader = glCreateShader( GL_VERTEX_SHADER );

	const QString		VS =
			"#version 330 core\n"
			"layout( location = 0 ) in vec2 vertex;\n"
			"out vec2 isf_FragNormCoord;\n"
			"void main(void)\n"
			"{\n"
			"   gl_Position = vec4( vertex, 0, 1 );\n"
			"	isf_FragNormCoord = ( vertex * 0.5 ) + 0.5;\n"
			"	isf_FragNormCoord.y = 1.0 - isf_FragNormCoord.y;\n"
			"}";

	const char			*VertexSourceArray = VS.toLatin1().constData();

	glShaderSource( VertexShader, 1, &VertexSourceArray, NULL );

	glCompileShader( VertexShader );

	glGetShaderiv( VertexShader, GL_COMPILE_STATUS, &Result );
	glGetShaderiv( VertexShader, GL_INFO_LOG_LENGTH, &InfoLogLength );

	if( Result == GL_FALSE )
	{
		QByteArray		InfoLogMessage;

		InfoLogMessage.resize( InfoLogLength );

		glGetShaderInfoLog( VertexShader, InfoLogLength, NULL, InfoLogMessage.data() );

		qWarning() << "Vertex Shader:" << QString::fromLatin1( InfoLogMessage );

		return;
	}

	GLuint		FragmentShader = glCreateShader( GL_FRAGMENT_SHADER );

	QString		FragmentSource =
			"#version 330 core\n"
			"#define gl_FragColor isf_OutputColour\n"
			"#define IMG_SIZE(i) textureSize(i,0)\n"
			"#define IMG_PIXEL(i,p) texture(i,p)\n"
			"#define IMG_NORM_PIXEL(i,p) texture2DRect(i,p)\n"
			"#define IMG_THIS_PIXEL(img) IMG_PIXEL(img,isf_FragNormCoord)\n"
			"#define IMG_THIS_NORM_PIXEL(img) IMG_NORM_PIXEL(img,isf_FragNormCoord*IMG_SIZE(img))\n"
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

			case IMAGE:
				FragmentSource.append( QString( "uniform sampler2D %1;\n" ).arg( it.key() ) );
				break;

			case COLOR:
				FragmentSource.append( QString( "uniform vec4 %1;\n" ).arg( it.key() ) );
				break;

			case AUDIO:
				FragmentSource.append( QString( "uniform float %1;\n" ).arg( it.key() ) );
				break;

			case AUDIOFFT:
				FragmentSource.append( QString( "uniform float %1;\n" ).arg( it.key() ) );
				break;

			case UNKNOWN:
				break;
		}
	}

	FragmentSource.append( mShaderSource );

	qDebug() << FragmentSource.split( '\n' );

	const char			*FragmentSourceArray = FragmentSource.toLatin1().constData();

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

		return;
	}

	GLuint		Program = glCreateProgram();

	glAttachShader( Program, VertexShader );
	glAttachShader( Program, FragmentShader );

	glLinkProgram( Program );

	glGetShaderiv( Program, GL_LINK_STATUS, &Result );
	glGetShaderiv( Program, GL_INFO_LOG_LENGTH, &InfoLogLength );

	if( Result == GL_FALSE )
	{
		QByteArray		InfoLogMessage;

		InfoLogMessage.resize( InfoLogLength );

		glGetShaderInfoLog( Program, InfoLogLength, NULL, InfoLogMessage.data() );

		qWarning() << "Shader Link:" << QString::fromLatin1( InfoLogMessage );

		glDeleteShader( VertexShader );
		glDeleteShader( FragmentShader );

		return;
	}

	if( mProgram )
	{
		glDeleteProgram( mProgram );
	}

	mProgram = Program;
}

void ISFNode::render( qint64 pTimeStamp, QUuid pSourcePinId )
{
	Q_UNUSED( pSourcePinId )

	if( !ISFPlugin::hasContextStatic() )
	{
		return;
	}

	GLint		Viewport[ 4 ];

	glGetIntegerv( GL_VIEWPORT, Viewport );

	if( !mVAO && GLEW_ARB_vertex_array_object )
	{
		glGenVertexArrays( 1, &mVAO );
	}

	if( mVAO )
	{
		glBindVertexArray( mVAO );
	}

	if( !mBuffer && GLEW_VERSION_1_5 )
	{
		GLfloat		Verticies[][ 2 ] = {
			{ -1,  1 },
			{ -1, -1 },
			{  1,  1 },
			{  1, -1 }
		};

		glGenBuffers( 1, &mBuffer );
		glBindBuffer( GL_ARRAY_BUFFER, mBuffer );
		glBufferData( GL_ARRAY_BUFFER, sizeof( Verticies ), Verticies, GL_STATIC_DRAW );
	}

	if( mProgram )
	{
		glClearColor( 0.0f, 0.0f, 0.4f, 0.0f );

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		glEnableVertexAttribArray( 0 );

		glBindBuffer( GL_ARRAY_BUFFER, mBuffer );

		glVertexAttribPointer(
		   0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		   2,                  // size
		   GL_FLOAT,           // type
		   GL_FALSE,           // normalized?
		   0,                  // stride
		   (void*)0            // array buffer offset
		);

		glUseProgram( mProgram );

		for( QMap<QString,ISFInput>::iterator it = mISFInputs.begin() ; it != mISFInputs.end() ; it++ )
		{
			GLint	UniLoc = glGetUniformLocation( mProgram, it.key().toLatin1().constData() );

			if( UniLoc == -1 )
			{
				continue;
			}

			QSharedPointer<fugio::PinInterface> P = mNode->findInputPinByName( it.key() );

			if( !P )
			{
				continue;
			}

			if( !P->isUpdated( mLastRenderTime ) )
			{
				if( it.value().mType == EVENT && it.value().mEventFlag )
				{
					glUniform1i( UniLoc, false );

					it.value().mEventFlag = false;
				}

				continue;
			}

			switch( it.value().mType )
			{
				case UNKNOWN:
					break;

				case EVENT:
					glUniform1i( UniLoc, true );

					it.value().mEventFlag = true;
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
							glUniform1i( UniLoc, 0 );

							glActiveTexture( GL_TEXTURE0 + 0 );

							TexInf->srcBind();
						}
					}
					break;
			}
		}

		GLint	RENDERSIZE = glGetUniformLocation( mProgram, "RENDERSIZE" );

		if( RENDERSIZE != -1 )
		{
			glUniform2f( RENDERSIZE, Viewport[ 2 ], Viewport[ 3 ] );
		}

		GLint	PASSINDEX = glGetUniformLocation( mProgram, "PASSINDEX" );

		for( int Pass = 0 ; Pass < 1 ; Pass++ )
		{
			if( PASSINDEX != -1 )
			{
				glUniform1i( PASSINDEX, Pass );
			}

			glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
		}

		glDisableVertexAttribArray( 0 );

		glUseProgram( 0 );

		glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

		mLastRenderTime = pTimeStamp;
	}

	if( mVAO )
	{
		glBindVertexArray( 0 );
	}
}


