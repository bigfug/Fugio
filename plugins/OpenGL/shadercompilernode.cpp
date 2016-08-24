#include "shadercompilernode.h"

#include <fugio/core/uuid.h>
#include <fugio/opengl/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/performance.h>

#include "syntaxhighlighterglsl.h"
#include "shaderpin.h"
#include "openglplugin.h"

ShaderCompilerNode::ShaderCompilerNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mProgramId( 0 ), mProgramLinked( false ), mLastShaderLoad( 0 ), mShaderVertId( 0 ), mShaderGeomId( 0 ), mShaderTessCtrlId( 0 ),
	  mShaderTessEvalId( 0 ), mShaderFragId( 0 )
{
	FUGID( PIN_VARYINGS,	"de12f397-d3ba-4d2e-9f53-5da4ed4bff37" );
	FUGID( PIN_BUFFER_MODE,	"0A134F4F-4A33-4E74-98AF-F89AFC4FB19D" );

	mPinShaderVertex   = pinInput( "Vertex" );
	mPinShaderTessCtrl = pinInput( "Tess Ctrl" );
	mPinShaderTessEval = pinInput( "Tess Eval" );
	mPinShaderGeometry = pinInput( "Geometry" );
	mPinShaderFragment = pinInput( "Fragment" );

	mPinInputVaryings = pinInput( "Varyings", PIN_VARYINGS );

	mValInputBufferMode = pinInput<fugio::ChoiceInterface *>( "Mode", mPinInputBufferMode, PID_CHOICE, PIN_BUFFER_MODE );

	QStringList		BufferModeChoices;

	BufferModeChoices << "GL_INTERLEAVED_ATTRIBS";
	BufferModeChoices << "GL_SEPARATE_ATTRIBS";

	mValInputBufferMode->setChoices( BufferModeChoices );

	mPinShaderVertex->registerInterface( IID_SYNTAX_HIGHLIGHTER,   new ShaderHighlighter( this ) );
	mPinShaderTessCtrl->registerInterface( IID_SYNTAX_HIGHLIGHTER, new ShaderHighlighter( this ) );
	mPinShaderTessEval->registerInterface( IID_SYNTAX_HIGHLIGHTER, new ShaderHighlighter( this ) );
	mPinShaderGeometry->registerInterface( IID_SYNTAX_HIGHLIGHTER, new ShaderHighlighter( this ) );
	mPinShaderFragment->registerInterface( IID_SYNTAX_HIGHLIGHTER, new ShaderHighlighter( this ) );

	mPinInputVaryings->registerPinInputType( PID_STRING );

	mOutputShader = pinOutput<OpenGLShaderInterface *>( "Shader", mOutputPinShader, PID_OPENGL_SHADER );

	qobject_cast<ShaderPin *>( mOutputPinShader->control()->qobject() )->setParent( this );

	mPinShaderVertex->setDescription( tr( "The source code for an OpenGL vertex shader - use a Text Editor node, or load from a file" ) );
	mPinShaderTessCtrl->setDescription( tr( "The source code for an OpenGL tessellation control shader - use a Text Editor node, or load from a file" ) );
	mPinShaderTessEval->setDescription( tr( "The source code for an OpenGL tessellation evaluation shader - use a Text Editor node, or load from a file" ) );
	mPinShaderGeometry->setDescription( tr( "The source code for an OpenGL geometry shader - use a Text Editor node, or load from a file" ) );
	mPinShaderFragment->setDescription( tr( "The source code for an OpenGL fragment shader - use a Text Editor node, or load from a file" ) );

	mOutputPinShader->setDescription( tr( "The compiled shader made from all of the source code inputs" ) );
}

ShaderCompilerNode::~ShaderCompilerNode( void )
{
	clearShader();
}

bool ShaderCompilerNode::initialise()
{
	if( !fugio::NodeControlBase::initialise() )
	{
		return( false );
	}

	if( !OpenGLPlugin::hasContextStatic() )
	{
		return( false );
	}

	return( true );
}

bool ShaderCompilerNode::deinitialise()
{
	return( NodeControlBase::deinitialise() );
}

void ShaderCompilerNode::inputsUpdated( qint64 pTimeStamp )
{
	fugio::Performance	Perf( mNode, "inputsUpdated", pTimeStamp );

	OPENGL_PLUGIN_DEBUG;

	if( mPinShaderVertex->isUpdated( mLastShaderLoad ) ||
		mPinShaderGeometry->isUpdated( mLastShaderLoad ) ||
		mPinShaderTessCtrl->isUpdated( mLastShaderLoad ) ||
		mPinShaderTessEval->isUpdated( mLastShaderLoad ) ||
		mPinShaderFragment->isUpdated( mLastShaderLoad ) ||
		mPinInputVaryings->isUpdated( mLastShaderLoad ) ||
		mPinInputBufferMode->isUpdated( mLastShaderLoad ) )
	{
		loadShader();

		mLastShaderLoad = pTimeStamp;
	}

	OPENGL_PLUGIN_DEBUG;
}

//QStringList OpenGLShaderCompiler::availableInputPins() const
//{
//	QStringList		SL;

//	SL.append( mUniformNames );
//	SL.append( mAttributeNames );

//	return( SL );
//}

//QList<QUuid> OpenGLShaderCompiler::pinAddTypesInput() const
//{
//	return( mNode->context()->global()->pinIds().keys() );
//}

//bool OpenGLShaderCompiler::canAcceptPin( fugio::PinInterface *pPin ) const
//{
//	return( pPin->direction() == PIN_OUTPUT );
//}

void ShaderCompilerNode::loadShader( QSharedPointer<fugio::PinInterface> pPin, GLuint &pShaderId, GLenum pShaderType, int &pCompiled, int &pFailed )
{
	QSharedPointer<fugio::PinInterface>	DstPin = pPin->connectedPin();

	if( DstPin )
	{
		QSharedPointer<PinControlInterface>		DstCtl = DstPin->control();

		if( DstCtl )
		{
			VariantInterface	*DstVar = qobject_cast<VariantInterface *>( DstCtl->qobject() );

			if( DstVar )
			{
				QByteArray		 Source    = DstVar->variant().toByteArray();
				const GLchar	*SourcePtr = Source.data();

				if( !Source.isEmpty() )
				{
					if( ( pShaderId = glCreateShader( pShaderType ) ) != 0 )
					{
						GLint			 Result;

						QObject				*O = pPin->findInterface( IID_SYNTAX_HIGHLIGHTER );
						ShaderHighlighter	*H = qobject_cast<ShaderHighlighter *>( O );

						glShaderSource( pShaderId, 1, &SourcePtr, 0 );

#if !defined( GL_ES_VERSION_2_0 )
						if( GLEW_ARB_shading_language_include )
						{
							glCompileShaderIncludeARB( pShaderId, 0, NULL, NULL );
						}
						else
#endif
						{
							glCompileShader( pShaderId );
						}

						glGetShaderiv( pShaderId, GL_INFO_LOG_LENGTH, &Result );

						QVector<GLchar>	Log( Result + 1 );

						if( Result > 0 )
						{
							glGetShaderInfoLog( pShaderId, Result, &Result, Log.data() );

							if( H )
							{
								H->setErrors( QString( Log.data() ) );

#if defined( OPENGL_DEBUG_ENABLE )
								qWarning() << QString( Log.data() );
#endif
							}
							else
							{
								qWarning() << QString( Log.data() );
							}
						}

						glGetShaderiv( pShaderId, GL_COMPILE_STATUS, &Result );

						if( Result == GL_TRUE )
						{
							glAttachShader( mProgramId, pShaderId );

							H->clearErrors();

							pCompiled++;

							return;
						}

						//if( !QOpenGLContext::currentContext()->format().testOption( QSurfaceFormat::DebugContext ) )
						{
							qWarning() << mNode->name() << QString( Log.data() );
						}

						glDeleteShader( pShaderId );

						pShaderId = 0;

						pFailed++;
					}
				}
			}
		}
	}
}

void ShaderCompilerNode::loadShader()
{
	GLint			Compiled = 0;
	GLint			Failed   = 0;

	clearShader();

	if( ( mProgramId = glCreateProgram() ) == 0 )
	{
		return;
	}

	OPENGL_PLUGIN_DEBUG;

	loadShader( mPinShaderVertex, mShaderVertId, GL_VERTEX_SHADER, Compiled, Failed );

	OPENGL_PLUGIN_DEBUG;

#if defined( GL_TESS_CONTROL_SHADER )
	loadShader( mPinShaderTessCtrl, mShaderTessCtrlId, GL_TESS_CONTROL_SHADER, Compiled, Failed );

	OPENGL_PLUGIN_DEBUG;
#endif

#if defined( GL_TESS_EVALUATION_SHADER )
	loadShader( mPinShaderTessEval, mShaderTessEvalId, GL_TESS_EVALUATION_SHADER, Compiled, Failed );

	OPENGL_PLUGIN_DEBUG;
#endif

#if defined( GL_GEOMETRY_SHADER )
	loadShader( mPinShaderGeometry, mShaderGeomId, GL_GEOMETRY_SHADER, Compiled, Failed );
#endif

	OPENGL_PLUGIN_DEBUG;

	loadShader( mPinShaderFragment, mShaderFragId, GL_FRAGMENT_SHADER, Compiled, Failed );

	OPENGL_PLUGIN_DEBUG;

	if( !Compiled )
	{
		mNode->setStatus( fugio::NodeInterface::Error );
		mNode->setStatusMessage( tr( "No shaders compiled" ) );
	}

	if( Failed )
	{
		mNode->setStatus( fugio::NodeInterface::Error );
		mNode->setStatusMessage( tr( "Shader compilation failed" ) );

		return;
	}

#if !defined( GL_ES_VERSION_2_0 )
	//-------------------------------------------------------------------------
	// Varyings

	QString		VaryingString = variant( mPinInputVaryings ).toString();
	QStringList	VaryingList;

	VaryingList = VaryingString.split( QRegExp( "\\W+" ), QString::SkipEmptyParts );

	if( !VaryingList.isEmpty() )
	{
		QList<QVector<char>>	VarDat;
		QVector<char *>			VarLst;

		for( QString S : VaryingList )
		{
			QVector<char>		VarNam;

			VarNam.resize( S.length() + 1 );

#if defined( strcpy_s )
			strcpy_s( VarNam.data(), VarNam.size(), S.toLatin1().constData() );
#else
			strcpy( VarNam.data(), S.toLatin1().constData() );
#endif
			VarDat << VarNam;
		}

		for( QVector<char> &VarNam : VarDat )
		{
			VarLst << VarNam.data();
		}

		QString		BufStr = variant( mPinInputBufferMode ).toString();
		GLenum		BufMod = GL_INTERLEAVED_ATTRIBS;

		if( BufStr == "GL_SEPARATE_ATTRIBS" )
		{
			BufMod = GL_SEPARATE_ATTRIBS;
		}

		if( GLEW_VERSION_3_0 )
		{
			glTransformFeedbackVaryings( mProgramId, VarLst.size(), VarLst.constData(), BufMod );

			OPENGL_PLUGIN_DEBUG;
		}
	}
#endif

	//-------------------------------------------------------------------------
	// Link

	glLinkProgram( mProgramId );

	OPENGL_PLUGIN_DEBUG;

	GLint			LinkLogLength = 0;

	glGetProgramiv( mProgramId, GL_INFO_LOG_LENGTH, &LinkLogLength );

	if( LinkLogLength > 0 )
	{
		QVector<GLchar>	Log( LinkLogLength + 1 );

		glGetProgramInfoLog( mProgramId, LinkLogLength, &LinkLogLength, Log.data() );

		mNode->setStatusMessage( QString( Log.constData() ) );
	}

	OPENGL_PLUGIN_DEBUG;

	GLint			LinkStatus = GL_FALSE;

	glGetProgramiv( mProgramId, GL_LINK_STATUS, &LinkStatus );

	if( LinkStatus != GL_TRUE )
	{
		mNode->setStatus( fugio::NodeInterface::Error );

		if( LinkLogLength <= 0 )
		{
#if !defined( GL_ES_VERSION_2_0 )
			if( !VaryingList.isEmpty() )
			{
				// NVIDIA cards fail to link but don't report anything!

				mNode->setStatusMessage( "Shader Linking Failed - Check Varyings" );
			}
			else
#endif
			{
				mNode->setStatusMessage( "Shader Linking Failed" );
			}
		}

		return;
	}

	mNode->setStatus( fugio::NodeInterface::Initialised );

	if( LinkLogLength <= 0 )
	{
		mNode->setStatusMessage( "Shader Linked" );
	}

	mProgramLinked = true;

	//-------------------------------------------------------------------------

	OPENGL_PLUGIN_DEBUG;

	processShader( mProgramId );

	OPENGL_PLUGIN_DEBUG;

	pinUpdated( mOutputPinShader );
}

void ShaderCompilerNode::clearShader()
{
	if( mShaderFragId )
	{
		glDeleteShader( mShaderFragId );

		mShaderFragId = 0;
	}

	if( mShaderGeomId )
	{
		glDeleteShader( mShaderGeomId );

		mShaderGeomId = 0;
	}

	if( mShaderTessEvalId )
	{
		glDeleteShader( mShaderTessEvalId );

		mShaderTessEvalId = 0;
	}

	if( mShaderTessCtrlId )
	{
		glDeleteShader( mShaderTessCtrlId );

		mShaderTessCtrlId = 0;
	}

	if( mShaderVertId )
	{
		glDeleteShader( mShaderVertId );

		mShaderVertId = 0;
	}

	if( mProgramId )
	{
		glDeleteProgram( mProgramId );

		mProgramId = 0;
	}

	mProgramLinked = false;
}

void ShaderCompilerNode::processShader( GLuint pProgramId )
{
	ShaderUniformData	UniformData;

	memset( &UniformData, 0, sizeof( UniformData ) );

	mShaderUniformTypes.clear();
	mShaderAttributeTypes.clear();

	mUniformNames.clear();
	mAttributeNames.clear();

	GLint		TextureBinding = 0;

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

#if defined( QT_DEBUG )
			//qDebug() << mNode->name() << i << ":" << UniformName << "size =" << UniformData.mSize << "type =" << UniformData.mType << "bind =" << UniformData.mTextureBinding << "loc =" << UniformData.mLocation;
#endif

			mUniformNames.append( UniformName );

			if( mShaderUniformTypes.contains( UniformName ) )//&& ( ShaderUniformTypes.value( UniformName ).first != UniformType || ShaderUniformTypes.value( UniformName ).first != UniformType )
			{
				continue;
			}

			switch( UniformData.mType )
			{
#if !defined( GL_ES_VERSION_2_0 )
				case GL_SAMPLER_1D:
				case GL_SAMPLER_1D_ARRAY:
				case GL_SAMPLER_1D_ARRAY_SHADOW:
				case GL_SAMPLER_1D_SHADOW:
				case GL_SAMPLER_2D_ARRAY:
				case GL_SAMPLER_2D_ARRAY_SHADOW:
				case GL_SAMPLER_2D_MULTISAMPLE:
				case GL_SAMPLER_2D_RECT:
				case GL_SAMPLER_2D_SHADOW:
				case GL_SAMPLER_3D:
#endif
				case GL_SAMPLER_2D:
				case GL_SAMPLER_CUBE:
#if defined( GL_SAMPLER_CUBE_MAP_ARRAY )
				case GL_SAMPLER_CUBE_MAP_ARRAY:
#endif
#if defined( GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW )
				case GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW:
#endif

#if !defined( GL_ES_VERSION_2_0 )
				case GL_SAMPLER_CUBE_SHADOW:
				case GL_INT_SAMPLER_1D:
				case GL_INT_SAMPLER_3D:
				case GL_INT_SAMPLER_CUBE:
				case GL_INT_SAMPLER_1D_ARRAY:
				case GL_INT_SAMPLER_2D_ARRAY:
				case GL_UNSIGNED_INT_SAMPLER_1D:
				case GL_UNSIGNED_INT_SAMPLER_2D:
				case GL_UNSIGNED_INT_SAMPLER_3D:
				case GL_UNSIGNED_INT_SAMPLER_CUBE:
				case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
				case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
				case GL_SAMPLER_2D_RECT_SHADOW:
				case GL_SAMPLER_BUFFER:
				case GL_INT_SAMPLER_2D_RECT:
				case GL_INT_SAMPLER_BUFFER:
				case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
				case GL_UNSIGNED_INT_SAMPLER_BUFFER:
				case GL_INT_SAMPLER_2D_MULTISAMPLE:
				case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
				case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
				case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
				case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
				case GL_INT_SAMPLER_CUBE_MAP_ARRAY:
				case GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY:
				case GL_SAMPLER:
#endif
					UniformData.mSampler = true;
					UniformData.mTextureBinding = ++TextureBinding;

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

			memset( &UniformData, 0, sizeof( UniformData ) );
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

			//qDebug() << mNode->name() << i << ":" << AttributeName << "(ATTRIBUTE) size =" << UniformData.mSize << "type =" << UniformData.mType << "loc =" << UniformData.mLocation;

			mAttributeNames.append( AttributeName );

			mShaderAttributeTypes.insert( AttributeName, UniformData );
		}
	}

	//emit updateUniformList( mUniformNames );
}


QSyntaxHighlighter *ShaderHighlighter::highlighter( QTextDocument *pDocument )
{
	if( !mHighlighter )
	{
		mHighlighter = new SyntaxHighlighterGLSL( pDocument );
	}

	return( mHighlighter );
}

void ShaderHighlighter::clearErrors()
{
	if( mHighlighter )
	{
		mHighlighter->clearErrors();
	}
}

void ShaderHighlighter::setErrors( const QString &pErrorText )
{
	if( mHighlighter )
	{
		mHighlighter->setErrors( pErrorText );
	}
}
