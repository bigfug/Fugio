#include "shadernode.h"

#include <QSettings>
#include <QDebug>
#include <QFile>

#include <fugio/fugio-base.h>

#include "fugio/interface_app.h"
#include "fugio/interface_context.h"

#include "fugio/interface_variant.h"
#include "fugio/interface_geometry.h"
#include "fugio/interface_colour.h"
#include <fugio/interface_opengl_state.h>
#include <fugio/interface_array.h>
#include <fugio/performance.h>

#include "openglplugin.h"
#include "openglbufferpin.h"

#include "syntaxhighlighterglsl.h"

ShaderNode::ShaderNode( QSharedPointer<InterfaceNode> pNode ) :
	NodeControlBase( pNode ), mProgramId( 0 ), mProgramLinked( false ), mFrameBufferId( 0 ), mLastShaderLoad( 0 ), mShaderGeomId( 0 ), mShaderTessCtrlId( 0 ),
	mShaderTessEvalId( 0 ), mShaderVertId( 0 ), mShaderFragId( 0 ),	mInitialised( false )
{
	mPinShaderVertex   = pinInput( "VertexShader" );
	mPinShaderTessCtrl = pinInput( "TessCtrlShader" );
	mPinShaderTessEval = pinInput( "TessEvalShader" );
	mPinShaderGeometry = pinInput( "GeometryShader" );
	mPinShaderFragment = pinInput( "FragmentShader" );

	mPinShaderGeometry->registerInterface( IID_SYNTAX_HIGHLIGHTER, this );
	mPinShaderVertex->registerInterface( IID_SYNTAX_HIGHLIGHTER, this );
	mPinShaderTessCtrl->registerInterface( IID_SYNTAX_HIGHLIGHTER, this );
	mPinShaderTessEval->registerInterface( IID_SYNTAX_HIGHLIGHTER, this );
	mPinShaderFragment->registerInterface( IID_SYNTAX_HIGHLIGHTER, this );

	mPinState    = pinInput( "State" );
	mPinGeometry = pinInput( "Geometry" );

	mOutputGeometry = pinOutput<InterfaceGeometry *>( "Output Geometry", mPinOutputGeometry, PID_GEOMETRY );

	mNode->addPin( mNode->context()->global()->createPin( "Output1", next_uuid(), mNode, PIN_OUTPUT ) );
}

ShaderNode::~ShaderNode( void )
{
	clearShader();
}

void ShaderNode::loadSettings( QSettings &pSettings )
{
	Q_UNUSED( pSettings )
}

void ShaderNode::saveSettings( QSettings &pSettings )
{
	Q_UNUSED( pSettings )
}

QSyntaxHighlighter *ShaderNode::highlighter( QTextDocument *pDocument )
{
	return( new SyntaxHighlighterGLSL( pDocument ) );
}

GLint ShaderNode::loadShader( QSharedPointer<InterfacePin> pPin, GLuint &pShaderId, GLenum pShaderType )
{
	GLint			Result = GL_TRUE;

	QSharedPointer<InterfacePin>	DstPin = pPin->connectedPin();

	if( DstPin != 0 )
	{
		QSharedPointer<InterfacePinControl>		DstCtl = DstPin->control();

		if( DstCtl != 0 )
		{
			InterfaceVariant	*DstVar = qobject_cast<InterfaceVariant *>( DstCtl->object() );

			if( DstVar != 0 )
			{
				Result = GL_FALSE;

				if( ( pShaderId = glCreateShader( pShaderType ) ) != 0 )
				{
					QByteArray		 Source    = DstVar->variant().toByteArray();
					const GLchar	*SourcePtr = Source.data();

					glShaderSource( pShaderId, 1, &SourcePtr, 0 );

					glCompileShader( pShaderId );

					glGetShaderiv( pShaderId, GL_COMPILE_STATUS, &Result );

					if( Result == GL_TRUE )
					{
						glAttachShader( mProgramId, pShaderId );
					}
					else
					{
						if( !QOpenGLContext::currentContext()->format().testOption( QSurfaceFormat::DebugContext ) )
						{
							glGetShaderiv( pShaderId, GL_INFO_LOG_LENGTH, &Result );

							QVector<GLchar>	Log( Result + 1 );

							glGetShaderInfoLog( pShaderId, Result, &Result, Log.data() );

							qWarning() << QString( Log.data() );
						}

						glDeleteShader( pShaderId );

						pShaderId = 0;
					}
				}
			}
		}
	}

	return( Result );
}

void ShaderNode::loadShader()
{
	GLint			Result;

	clearShader();

	if( ( mProgramId = glCreateProgram() ) == 0 )
	{
		return;
	}

	OPENGL_PLUGIN_DEBUG;

	loadShader( mPinShaderVertex, mShaderVertId, GL_VERTEX_SHADER );

	OPENGL_PLUGIN_DEBUG;

#if defined( GL_TESS_CONTROL_SHADER )
	loadShader( mPinShaderTessCtrl, mShaderTessCtrlId, GL_TESS_CONTROL_SHADER );

	OPENGL_PLUGIN_DEBUG;
#endif

#if defined( GL_TESS_EVALUATION_SHADER )
	loadShader( mPinShaderTessEval, mShaderTessEvalId, GL_TESS_EVALUATION_SHADER );

	OPENGL_PLUGIN_DEBUG;
#endif

	loadShader( mPinShaderGeometry, mShaderGeomId, GL_GEOMETRY_SHADER );

	OPENGL_PLUGIN_DEBUG;

	loadShader( mPinShaderFragment, mShaderFragId, GL_FRAGMENT_SHADER );

	OPENGL_PLUGIN_DEBUG;

	//-------------------------------------------------------------------------

	glLinkProgram( mProgramId );

	OPENGL_PLUGIN_DEBUG;

	glGetProgramiv( mProgramId, GL_LINK_STATUS, &Result );

	if( Result != GL_TRUE )
	{
		if( !QOpenGLContext::currentContext()->format().testOption( QSurfaceFormat::DebugContext ) )
		{
			glGetShaderiv( mProgramId, GL_INFO_LOG_LENGTH, &Result );

			QVector<GLchar>	Log( Result + 1 );

			glGetShaderInfoLog( mProgramId, Result, &Result, Log.data() );

			qWarning() << QString( Log.data() );
		}

		return;
	}

	mProgramLinked = true;

	//-------------------------------------------------------------------------

	OPENGL_PLUGIN_DEBUG;

	processShader( mProgramId );

	OPENGL_PLUGIN_DEBUG;
}

void ShaderNode::clearShader()
{
	if( mShaderFragId != 0 )
	{
		glDeleteShader( mShaderFragId );

		mShaderFragId = 0;
	}

	if( mShaderGeomId != 0 )
	{
		glDeleteShader( mShaderGeomId );

		mShaderGeomId = 0;
	}

	if( mShaderTessEvalId != 0 )
	{
		glDeleteShader( mShaderTessEvalId );

		mShaderTessEvalId = 0;
	}

	if( mShaderTessCtrlId != 0 )
	{
		glDeleteShader( mShaderTessCtrlId );

		mShaderTessCtrlId = 0;
	}

	if( mShaderVertId != 0 )
	{
		glDeleteShader( mShaderVertId );

		mShaderVertId = 0;
	}

	if( mProgramId != 0 )
	{
		glDeleteProgram( mProgramId );

		mProgramId     = 0;
		mProgramLinked = false;
	}
}

void ShaderNode::processShader( GLuint pProgramId )
{
	ShaderUniformData	UniformData;

	memset( &UniformData, 0, sizeof( UniformData ) );

	mShaderUniformTypes.clear();
	mShaderAttributeTypes.clear();

	mUniformNames.clear();
	mAttributeNames.clear();

	GLint		ActiveUniforms = 0;

	glGetProgramiv( pProgramId, GL_ACTIVE_UNIFORMS, &ActiveUniforms );

	if( ActiveUniforms > 0 )
	{
		GLint		ActiveUniformsMaxLength;

		glGetProgramiv( pProgramId, GL_ACTIVE_UNIFORM_MAX_LENGTH, &ActiveUniformsMaxLength );

		std::vector<GLchar>		Name( ActiveUniformsMaxLength );

		for( GLint i = 0 ; i < ActiveUniforms ; i++ )
		{
			GLsizei		NameLength;

			glGetActiveUniform( pProgramId, i, Name.size(), &NameLength, &UniformData.mSize, &UniformData.mType, &Name[ 0 ] );

			QString		UniformName = QString::fromLocal8Bit( &Name[ 0 ] );

			UniformData.mLocation = glGetUniformLocation( pProgramId, Name.data() );

			if( UniformData.mLocation == -1 )
			{
				continue;
			}

			qDebug() << mNode->name() << i << ":" << UniformName << "size =" << UniformData.mSize << "type =" << UniformData.mType << "bind =" << UniformData.mTextureBinding << "loc =" << UniformData.mLocation;

			mUniformNames.append( UniformName );

			if( mShaderUniformTypes.contains( UniformName ) )//&& ( ShaderUniformTypes.value( UniformName ).first != UniformType || ShaderUniformTypes.value( UniformName ).first != UniformType )
			{
				continue;
			}

			if( UniformData.mSize != 1 )
			{
				continue;
			}

			switch( UniformData.mType )
			{
				case GL_SAMPLER_1D:
				case GL_SAMPLER_1D_ARRAY:
				case GL_SAMPLER_1D_ARRAY_SHADOW:
				case GL_SAMPLER_1D_SHADOW:
				case GL_SAMPLER_2D:
				case GL_SAMPLER_2D_ARRAY:
				case GL_SAMPLER_2D_ARRAY_SHADOW:
				case GL_SAMPLER_2D_MULTISAMPLE:
				case GL_SAMPLER_2D_RECT:
				case GL_SAMPLER_2D_SHADOW:
				case GL_SAMPLER_3D:
				case GL_SAMPLER_CUBE:
#if defined( GL_SAMPLER_CUBE_MAP_ARRAY )
				case GL_SAMPLER_CUBE_MAP_ARRAY:
#endif
#if defined( GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW )
				case GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW:
#endif
				case GL_SAMPLER_CUBE_SHADOW:
					UniformData.mSampler = true;
					UniformData.mTextureBinding++;

					// fall through...

				case GL_FLOAT:
				case GL_FLOAT_VEC2:
				case GL_FLOAT_VEC3:
				case GL_FLOAT_VEC4:
				case GL_INT:
				case GL_INT_VEC2:
				case GL_INT_VEC3:
				case GL_INT_VEC4:
				case GL_BOOL:
				case GL_BOOL_VEC2:
				case GL_BOOL_VEC3:
				case GL_BOOL_VEC4:
				case GL_FLOAT_MAT2:
				case GL_FLOAT_MAT3:
				case GL_FLOAT_MAT4:
					mShaderUniformTypes.insert( UniformName, UniformData );
					break;

			}
		}
	}

	memset( &UniformData, 0, sizeof( UniformData ) );

	GLint		ActiveAtrributes = 0;

	glGetProgramiv( pProgramId, GL_ACTIVE_ATTRIBUTES, &ActiveAtrributes );

	if( ActiveAtrributes > 0 )
	{
		GLint		ActiveUniformsMaxLength;

		glGetProgramiv( pProgramId, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &ActiveUniformsMaxLength );

		std::vector<GLchar>		Name( ActiveUniformsMaxLength );

		for( GLint i = 0 ; i < ActiveAtrributes ; i++ )
		{
			GLsizei		NameLength;

			glGetActiveAttrib( pProgramId, i, Name.size(), &NameLength, &UniformData.mSize, &UniformData.mType, &Name[ 0 ] );

			QString		AttributeName = QString::fromLocal8Bit( &Name[ 0 ] );

			UniformData.mLocation = glGetAttribLocation( pProgramId, Name.data() );

			if( UniformData.mLocation == -1 )
			{
				continue;
			}

			qDebug() << mNode->name() << i << ":" << AttributeName << "(ATTRIBUTE) size =" << UniformData.mSize << "type =" << UniformData.mType << "loc =" << UniformData.mLocation;

			mAttributeNames.append( AttributeName );

			mShaderAttributeTypes.insert( AttributeName, UniformData );
		}
	}

	emit updateUniformList( mUniformNames );
}

bool ShaderNode::initialise( void )
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	if( !QOpenGLContext::currentContext() )
	{
		return( false );
	}

	if( !initializeOpenGLFunctions() )
	{
		qDebug() << "ShaderNode can't initialise OpenGL Functions";

		return( false );
	}

	mInitialised = true;

	return( true );
}

bool ShaderNode::deinitialise( void )
{
	mInitialised = false;

	return( true );
}

void ShaderNode::bindTexture( QList<ShaderBindData> &Bindings, QSharedPointer<InterfacePinControl> PinControl, ShaderUniformData &UniformData )
{
	InterfaceTexture		*PinTexture = qobject_cast<InterfaceTexture *>( PinControl->object() );

	if( PinTexture == 0 )
	{
		return;
	}

	ShaderBindData	BindData;

	BindData.mUnit    = GL_TEXTURE0 + UniformData.mTextureBinding;
	BindData.mTexture = PinTexture;

	if( BindData.mTexture && BindData.mTexture->textureId() )
	{
		Bindings.append( BindData );

		glActiveTexture( BindData.mUnit );

		BindData.mTexture->bind();

		//glEnable( BindData.mTarget );

		glUniform1i( UniformData.mLocation, UniformData.mTextureBinding );

		OPENGL_PLUGIN_DEBUG;

		//qDebug() << PIN->name() << "Bind GL_TEXTURE" << UniformData.mTextureBinding << UniformData.mLocation << BindData.mTarget << PinTexture->textureId();
	}
}

void ShaderNode::bindInputTextures( QList<ShaderBindData> &Bindings )
{
	for( ShaderUniformMap::iterator it = mShaderUniformTypes.begin() ; it != mShaderUniformTypes.end() ; it++ )
	{
		ShaderUniformData				&UniformData = it.value();
		QSharedPointer<InterfacePin>	PIN = mNode->findPinByName( it.key() );

		if( PIN == 0 )
		{
			continue;
		}

		if( !PIN->isConnected() )
		{
			continue;
		}

		QSharedPointer<InterfacePinControl>		PinControl = PIN->connectedPin()->control();

		if( PinControl == 0 )
		{
			continue;
		}

		if( UniformData.mSampler && UniformData.mSize == 1 )
		{
			bindTexture( Bindings, PinControl, UniformData );
		}
	}
}

void ShaderNode::updateUniforms( QList<ShaderBindData> &Bindings, qint64 pTimeStamp )
{
	bool		NumberOK;

	for( ShaderUniformMap::iterator it = mShaderUniformTypes.begin() ; it != mShaderUniformTypes.end() ; it++ )
	{
		ShaderUniformData				&UniformData = it.value();
		QSharedPointer<InterfacePin>	PIN = mNode->findPinByName( it.key() );

		if( PIN == 0 )
		{
			continue;
		}

		if( !PIN->isConnected() )
		{
			continue;
		}

		QSharedPointer<InterfacePinControl>		PinControl = PIN->connectedPin()->control();

		if( PinControl == 0 )
		{
			continue;
		}

		if( UniformData.mSampler && UniformData.mSize == 1 )
		{
			bindTexture( Bindings, PinControl, UniformData );

			continue;
		}

		//		if( mLastShaderLoad < pTimeStamp )//&& !PIN->isUpdated( pTimeStamp ) )
		//		{
		//			if( PIN->controlUuid() == PID_TRIGGER )
		//			{
		//				switch( UniformData.mType )
		//				{
		//					case GL_BOOL:
		//						glUniform1f( UniformData.mLocation, false );
		//						break;
		//				}
		//			}

		//			continue;
		//		}

		if( true )
		{
			InterfaceVariant		*PinVariant = qobject_cast<InterfaceVariant *>( PinControl->object() );

			if( PinVariant != 0 )
			{
				switch( UniformData.mType )
				{
					case GL_BOOL:
						{
							GLboolean		NewVal = PinVariant->variant().toBool();

							glUniform1ui( UniformData.mLocation, NewVal );
						}
						break;

					case GL_INT:
						{
							GLint		NewVal = PinVariant->variant().toInt( &NumberOK );

							if( NumberOK )
							{
								glUniform1i( UniformData.mLocation, NewVal );
							}
						}
						break;

					case GL_FLOAT:
						{
							GLfloat		NewVal = PinVariant->variant().toFloat( &NumberOK );

							if( NumberOK )
							{
								glUniform1f( UniformData.mLocation, NewVal );
							}
						}
						break;

					case GL_FLOAT_VEC2:
						{
							QVector2D		Vec2 = PinVariant->variant().value<QVector2D>();

							glUniform2f( UniformData.mLocation, Vec2.x(), Vec2.y() );
						}
						break;

					case GL_FLOAT_VEC3:
						{
							QVector3D		Vec3 = PinVariant->variant().value<QVector3D>();

							glUniform3f( UniformData.mLocation, Vec3.x(), Vec3.y(), Vec3.z() );
						}
						break;

					case GL_FLOAT_VEC4:
						{
							QVector4D		Vec4 = PinVariant->variant().value<QVector4D>();

							glUniform4f( UniformData.mLocation, Vec4.x(), Vec4.y(), Vec4.z(), Vec4.w() );
						}
						break;

					case GL_FLOAT_MAT2:
						{
							QMatrix4x4		Mat4 = PinVariant->variant().value<QMatrix4x4>();
							QMatrix2x2		Mat2 = Mat4.toGenericMatrix<2,2>();

							glUniformMatrix2fv( UniformData.mLocation, 1, GL_FALSE, Mat2.constData() );
						}
						break;

					case GL_FLOAT_MAT3:
						{
							QMatrix4x4		Mat4 = PinVariant->variant().value<QMatrix4x4>();
							QMatrix3x3		Mat3 = Mat4.toGenericMatrix<3,3>();

							//qDebug() << Mat4;
							//qDebug() << Mat3;

							glUniformMatrix3fv( UniformData.mLocation, 1, GL_FALSE, Mat3.constData() );
						}
						break;

					case GL_FLOAT_MAT4:
						{
							QMatrix4x4		NewVal = PinVariant->variant().value<QMatrix4x4>();

							glUniformMatrix4fv( UniformData.mLocation, 1, GL_FALSE, NewVal.constData() );
						}
						break;
				}

				OPENGL_PLUGIN_DEBUG;

				continue;
			}
		}

		if( true )
		{
			InterfaceColour		*PinColour = qobject_cast<InterfaceColour *>( PinControl->object() );

			if( PinColour != 0 )
			{
				switch( UniformData.mType )
				{
					case GL_FLOAT_VEC3:
						glUniform3f( UniformData.mLocation, PinColour->colour().redF(), PinColour->colour().greenF(), PinColour->colour().blueF() );
						break;

					case GL_FLOAT_VEC4:
						glUniform4f( UniformData.mLocation, PinColour->colour().redF(), PinColour->colour().greenF(), PinColour->colour().blueF(), PinColour->colour().alphaF() );
						break;
				}

				OPENGL_PLUGIN_DEBUG;

				continue;
			}
		}

		if( true )
		{
			if( PIN->controlUuid() == PID_TRIGGER )
			{
				switch( UniformData.mType )
				{
					case GL_BOOL:
						glUniform1f( UniformData.mLocation, true );
						break;
				}

				OPENGL_PLUGIN_DEBUG;

				continue;
			}
		}
	}
}

void ShaderNode::bindAttributes( void )
{
	for( ShaderUniformMap::iterator it = mShaderAttributeTypes.begin() ; it != mShaderAttributeTypes.end() ; it++ )
	{
		ShaderUniformData				&UniformData = it.value();
		QSharedPointer<InterfacePin>	PIN = mNode->findPinByName( it.key() );

		if( !PIN || !PIN->isConnected() )
		{
			continue;
		}

//		if( mLastShaderLoad < pTimeStamp && !PIN->isUpdated( pTimeStamp ) )
//		{
//			continue;
//		}

		QSharedPointer<InterfacePinControl>		PinControl = PIN->connectedPin()->control();

		if( PinControl == 0 )
		{
			continue;
		}

		OpenGLBufferPin					*Buffer = qobject_cast<OpenGLBufferPin *>( PinControl->object() );

		if( !Buffer )
		{
			continue;
		}

		switch( Buffer->mCurrType )
		{
			case QMetaType::Float:
				Buffer->buffer()->bind();

				glVertexAttribPointer( UniformData.mLocation, 1, GL_FLOAT, GL_FALSE, 0, nullptr );

				glEnableVertexAttribArray( UniformData.mLocation );

				if( Buffer->mInstanced )
				{
					glVertexAttribDivisor( UniformData.mLocation, 1 );
				}
				break;

			case QMetaType::QVector2D:
			case QMetaType::QPointF:
				Buffer->buffer()->bind();

				glVertexAttribPointer( UniformData.mLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr );

				glEnableVertexAttribArray( UniformData.mLocation );

				if( Buffer->mInstanced )
				{
					glVertexAttribDivisor( UniformData.mLocation, 1 );
				}
				break;

			case QMetaType::QVector3D:
				Buffer->buffer()->bind();

				glVertexAttribPointer( UniformData.mLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

				glEnableVertexAttribArray( UniformData.mLocation );

				if( Buffer->mInstanced )
				{
					glVertexAttribDivisor( UniformData.mLocation, 1 );
				}
				break;

			case QMetaType::QVector4D:
				Buffer->buffer()->bind();

				glVertexAttribPointer( UniformData.mLocation, 4, GL_FLOAT, GL_FALSE, 0, nullptr );

				glEnableVertexAttribArray( UniformData.mLocation );

				if( Buffer->mInstanced )
				{
					glVertexAttribDivisor( UniformData.mLocation, 1 );
				}
				break;

			case QMetaType::QMatrix4x4:
				Buffer->buffer()->bind();

				for( int i = 0 ; i < 4 ; i++ )
				{
					glVertexAttribPointer( UniformData.mLocation + i, 4, GL_FLOAT, GL_FALSE, sizeof( GLfloat ) * 4 * 4, BUFFER_OFFSET( sizeof( GLfloat ) * 4 * i ) );

					glEnableVertexAttribArray( UniformData.mLocation + i );

					if( Buffer->mInstanced )
					{
						glVertexAttribDivisor( UniformData.mLocation + i, 1 );
					}
				}
				break;
		}


		if( !Buffer->mInstanced )
		{
			glVertexAttribDivisor( UniformData.mLocation, 0 );
		}
	}
}

void ShaderNode::releaseAttributes()
{
	for( ShaderUniformMap::iterator it = mShaderAttributeTypes.begin() ; it != mShaderAttributeTypes.end() ; it++ )
	{
		ShaderUniformData				&UniformData = it.value();
		QSharedPointer<InterfacePin>	PIN = mNode->findPinByName( it.key() );

		if( !PIN || !PIN->isConnected() )
		{
			continue;
		}

//		if( mLastShaderLoad < pTimeStamp && !PIN->isUpdated( pTimeStamp ) )
//		{
//			continue;
//		}

		QSharedPointer<InterfacePinControl>		PinControl = PIN->connectedPin()->control();

		if( PinControl == 0 )
		{
			continue;
		}

		OpenGLBufferPin					*Buffer = qobject_cast<OpenGLBufferPin *>( PinControl->object() );

		if( !Buffer )
		{
			continue;
		}

		switch( UniformData.mType )
		{
			case QMetaType::QMatrix4x4:
				for( int i = 0 ; i < 4 ; i++ )
				{
					glDisableVertexAttribArray( UniformData.mLocation + i );
				}
				break;

			default:
				glDisableVertexAttribArray( UniformData.mLocation );
				break;
		}
	}
}

void ShaderNode::bindOutputBuffers( QVector<GLenum> &Buffers, QList< QSharedPointer<InterfacePin> > &OutPinLst, int &W, int &H, int &D )
{
	for( QList< QSharedPointer<InterfacePin> >::iterator it = OutPinLst.begin() ; it != OutPinLst.end() ; it++ )
	{
		QSharedPointer<InterfacePin>	OutPin = *it;

		if( !OutPin->isConnected() )
		{
			continue;
		}

		QSharedPointer<InterfacePin>			DstPin = OutPin->connectedPin();

		if( DstPin == 0 )
		{
			continue;
		}

		QSharedPointer<InterfacePinControl>		DstControl = DstPin->control();

		if( DstControl == 0 )
		{
			continue;
		}

		InterfaceTexture		*DstTexture = qobject_cast<InterfaceTexture *>( DstControl->object() );

		if( DstTexture == 0 )
		{
			continue;
		}

		if( DstTexture->textureId() == 0 )
		{
			DstTexture->update( 0, 0 );

			if( DstTexture->textureId() == 0 )
			{
				continue;
			}
		}

		if( Buffers.empty() )
		{
			W = DstTexture->size().at( 0 );
			H = DstTexture->size().at( 1 );
			D = DstTexture->size().at( 2 );
		}

		//glBindTexture( DstTexture->target(), 0 );

		if( mFrameBufferId == 0 )
		{
			glGenFramebuffers( 1, &mFrameBufferId );

			if( mFrameBufferId == 0 )
			{
				continue;
			}
		}

		glBindFramebuffer( GL_FRAMEBUFFER, mFrameBufferId );

		switch( DstTexture->target() )
		{
			case GL_TEXTURE_1D:
				glFramebufferTexture1D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + Buffers.size(), DstTexture->target(), DstTexture->textureId(), 0 );
				break;

			case GL_TEXTURE_2D:
			case GL_TEXTURE_RECTANGLE:
				glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + Buffers.size(), DstTexture->target(), DstTexture->textureId(), 0 );
				break;

			case GL_TEXTURE_3D:
				glFramebufferTexture3D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + Buffers.size(), DstTexture->target(), DstTexture->textureId(), 0, 0 );
				break;
		}

		OPENGL_PLUGIN_DEBUG;

		Buffers.append( GL_COLOR_ATTACHMENT0 + Buffers.size() );
	}
}

void ShaderNode::releaseInputTextures( QList<ShaderBindData> &Bindings )
{
	for( QList<ShaderBindData>::iterator it = Bindings.begin() ; it != Bindings.end() ; it++ )
	{
		ShaderBindData	&BindData = *it;

		glActiveTexture( BindData.mUnit );

		BindData.mTexture->release();
	}

	glActiveTexture( GL_TEXTURE0 );
}

void ShaderNode::drawGeometry()
{
	fugio::Performance	Perf( mNode, "drawGeometry" );

	if( !mInitialised )
	{
		return;
	}

	if( !mProgramLinked )
	{
		return;
	}

	QList< QSharedPointer<InterfacePin> >	InpPinLst = mNode->enumInputPins();

	//-------------------------------------------------------------------------

	glUseProgram( mProgramId );

	OPENGL_PLUGIN_DEBUG;

	QList<ShaderBindData>		Bindings;

	bindInputTextures( Bindings );

	bindAttributes();

	//-------------------------------------------------------------------------

	InterfaceOpenGLState		*CurrentState = 0;
	InterfaceOpenGLState		*NextState;
	InterfaceGeometry			*Geometry;

	//-------------------------------------------------------------------------

	for( QList< QSharedPointer<InterfacePin> >::iterator it = InpPinLst.begin() ; it != InpPinLst.end() ; it++ )
	{
		QSharedPointer<InterfacePin>	InpPin = *it;

		if( !InpPin->isConnectedToActiveNode() )
		{
			continue;
		}

		QSharedPointer<InterfacePinControl>		 PinCtl = InpPin->connectedPin()->control();

		if( PinCtl.isNull() )
		{
			continue;
		}

		if( ( Geometry = qobject_cast<InterfaceGeometry *>( PinCtl->object() ) ) != 0 )
		{
			Geometry->drawGeometry();

			continue;
		}

		if( ( NextState = qobject_cast<InterfaceOpenGLState *>( PinCtl->object() ) ) != 0 )
		{
			if( CurrentState != 0 )
			{
				CurrentState->stateEnd();
			}

			CurrentState = NextState;

			CurrentState->stateBegin();
		}
	}

	if( CurrentState != 0 )
	{
		CurrentState->stateEnd();
	}

	releaseAttributes();

	//-------------------------------------------------------------------------

	glUseProgram( 0 );

	//-------------------------------------------------------------------------

	releaseInputTextures( Bindings );

	OPENGL_PLUGIN_DEBUG;
}

int ShaderNode::activeBufferCount( QList< QSharedPointer<InterfacePin> > &OutPinLst ) const
{
	int		ActiveBufferCount = 0;

	for( QList< QSharedPointer<InterfacePin> >::iterator it = OutPinLst.begin() ; it != OutPinLst.end() ; it++ )
	{
		QSharedPointer<InterfacePin>	OutPin = *it;

		if( !OutPin->isConnected() )
		{
			continue;
		}

		QSharedPointer<InterfacePin>			DstPin = OutPin->connectedPin();

		if( DstPin == 0 )
		{
			continue;
		}

		QSharedPointer<InterfacePinControl>		DstControl = DstPin->control();

		if( DstControl == 0 )
		{
			continue;
		}

		InterfaceTexture		*DstTexture = qobject_cast<InterfaceTexture *>( DstControl->object() );

		if( DstTexture == 0 )
		{
			continue;
		}

		ActiveBufferCount++;
	}

	return ActiveBufferCount;
}

void ShaderNode::inputsUpdated( qint64 pTimeStamp )
{
	fugio::Performance	Perf( mNode, "inputsUpdated" );

	if( !mInitialised )
	{
		return;
	}

	OPENGL_PLUGIN_DEBUG;

	if( mPinShaderVertex->isUpdated( mLastShaderLoad ) || mPinShaderGeometry->isUpdated( mLastShaderLoad ) || mPinShaderFragment->isUpdated( mLastShaderLoad ) )
	{
		loadShader();

		mLastShaderLoad = pTimeStamp;
	}

	OPENGL_PLUGIN_DEBUG;

	if( !mProgramLinked )
	{
		return;
	}

	//-------------------------------------------------------------------------

	int		W, H, D;

	QList< QSharedPointer<InterfacePin> >	OutPinLst = mNode->enumOutputPins();
	QList< QSharedPointer<InterfacePin> >	InpPinLst = mNode->enumInputPins();

	//-------------------------------------------------------------------------

	glUseProgram( mProgramId );

	OPENGL_PLUGIN_DEBUG;

	QList<ShaderBindData>		Bindings;

	updateUniforms( Bindings, pTimeStamp );

	if( mPinOutputGeometry->isConnectedToActiveNode() )
	{
		mNode->context()->pinUpdated( mPinOutputGeometry );
	}

	if( activeBufferCount( OutPinLst ) == 0 )
	{
		glUseProgram( 0 );

		return;
	}

	//-------------------------------------------------------------------------
	// Bind all output buffers

	QVector<GLenum>		Buffers;

	bindOutputBuffers( Buffers, OutPinLst, W, H, D );

	if( mFrameBufferId != 0 )
	{
		glBindFramebuffer( GL_FRAMEBUFFER, mFrameBufferId );

		if( !Buffers.empty() )
		{
			glDrawBuffers( Buffers.size(), Buffers.data() );

			OPENGL_PLUGIN_DEBUG;
		}

		if( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
		{
			qDebug() << "glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE";
		}
	}

	//-------------------------------------------------------------------------

	InterfaceOpenGLState		*State = 0;

	if( mPinState->isConnected() )
	{
		State = qobject_cast<InterfaceOpenGLState *>( mPinState->connectedPin()->control()->object() );
	}

	if( State != 0 )
	{
		State->stateBegin();
	}

	//-------------------------------------------------------------------------

	if( true )
	{
		glViewport( 0, 0, W, H );

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		QList<InterfaceGeometry*>		GeometryList;

		for( QList< QSharedPointer<InterfacePin> >::iterator it = InpPinLst.begin() ; it != InpPinLst.end() ; it++ )
		{
			QSharedPointer<InterfacePin>	InpPin = *it;

			if( !InpPin->isConnectedToActiveNode() )
			{
				continue;
			}

			QSharedPointer<InterfacePinControl>		 GeometryControl = InpPin->connectedPin()->control();

			InterfaceGeometry		*Geometry = ( GeometryControl.isNull() ? 0 : qobject_cast<InterfaceGeometry *>( GeometryControl->object() ) );

			if( Geometry != 0 )
			{
				GeometryList.append( Geometry );
			}
		}

		if( GeometryList.isEmpty() )
		{
			QMatrix4x4		pmvMatrix;

			pmvMatrix.setToIdentity();

			pmvMatrix.ortho( QRect( QPoint( 0, 0 ), QSize( W, H ) ) );

			glMatrixMode( GL_PROJECTION );
			glLoadIdentity();

			glLoadMatrixf( pmvMatrix.constData() );

			//Initialize Modelview Matrix
			glMatrixMode( GL_MODELVIEW );
			glLoadIdentity();

			glColor4f( 1.0, 1.0, 1.0, 1.0 );

			//glEnable( GL_BLEND );

			//glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

			//glActiveTexture( GL_TEXTURE0 );

			OPENGL_PLUGIN_DEBUG;

			GLint		x0 = 0;
			GLint		x1 = W;
			GLint		y0 = 0;
			GLint		y1 = H;

			glBegin( GL_QUADS );
				glMultiTexCoord2i( GL_TEXTURE0, x0, y1 );	glMultiTexCoord2f( GL_TEXTURE1, 0, 1 );		glVertex2i( x0, y0 );
				glMultiTexCoord2i( GL_TEXTURE0, x1, y1 );	glMultiTexCoord2f( GL_TEXTURE1, 1, 1 );		glVertex2i( x1, y0 );
				glMultiTexCoord2i( GL_TEXTURE0, x1, y0 );	glMultiTexCoord2f( GL_TEXTURE1, 1, 0 );		glVertex2i( x1, y1 );
				glMultiTexCoord2i( GL_TEXTURE0, x0, y0 );	glMultiTexCoord2f( GL_TEXTURE1, 0, 0 );		glVertex2i( x0, y1 );
			glEnd();
		}
		else
		{
			glMatrixMode( GL_PROJECTION );
			glLoadIdentity();

			//Initialize Modelview Matrix
			glMatrixMode( GL_MODELVIEW );
			glLoadIdentity();

			glColor4f( 1.0, 1.0, 1.0, 1.0 );

			//glEnable( GL_BLEND );

			//glBlendFunc( GL_ONE, GL_ONE );

			//glActiveTexture( GL_TEXTURE0 );

			OPENGL_PLUGIN_DEBUG;

			//glTranslatef( 0.0f, 0.0f, -5.0f );

			foreach( InterfaceGeometry *Geometry, GeometryList )
			{
				Geometry->drawGeometry();
			}
		}

		OPENGL_PLUGIN_DEBUG;
	}

	//-------------------------------------------------------------------------

	glUseProgram( 0 );

	if( State != 0 )
	{
		State->stateEnd();
	}

	//-------------------------------------------------------------------------

	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	//-------------------------------------------------------------------------

	releaseInputTextures( Bindings );

	//-------------------------------------------------------------------------

	for( QList< QSharedPointer<InterfacePin> >::iterator it = OutPinLst.begin() ; it != OutPinLst.end() ; it++ )
	{
		QSharedPointer<InterfacePin>	OutPin = *it;

		if( !OutPin->isConnectedToActiveNode() )
		{
			continue;
		}

		QSharedPointer<InterfacePin>			DstPin = OutPin->connectedPin();

		if( DstPin == 0 )
		{
			continue;
		}

		QSharedPointer<InterfacePinControl>		DstControl = DstPin->control();

		if( DstControl == 0 )
		{
			continue;
		}

		InterfaceTexture		*DstTexture = qobject_cast<InterfaceTexture *>( DstControl->object() );

		if( DstTexture == 0 )
		{
			continue;
		}

		mNode->context()->pinUpdated( OutPin );
	}

	OPENGL_PLUGIN_DEBUG;
}

void ShaderNode::uniformAdd( QString pUniform )
{
	pinInput( pUniform );
}

QStringList ShaderNode::availableInputPins() const
{
	QStringList		SL;

	SL.append( mUniformNames );
	SL.append( mAttributeNames );

	return( SL );
}

QList<QUuid> ShaderNode::pinAddTypesInput() const
{
	static QList<QUuid>		PinLst;

	if( PinLst.isEmpty() )
	{
		PinLst << PID_GEOMETRY;
		PinLst << PID_OPENGL_STATE;
		PinLst << PID_OPENGL_BUFFER;
	}

	return( PinLst );
}

bool ShaderNode::canAcceptPin( InterfacePin *pPin ) const
{
	if( pPin->direction() == PIN_INPUT )
	{
		return( false );
	}

	return( pinAddTypesInput().contains( pPin->controlUuid() ) );
}
