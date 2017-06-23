#include "shadercompilernode.h"

#include <fugio/core/uuid.h>
#include <fugio/opengl/uuid.h>
#include <fugio/text/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/performance.h>
#include <fugio/text/syntax_highlighter_interface.h>

#include "syntaxhighlighterglsl.h"
#include "shaderpin.h"
#include "openglplugin.h"

ShaderCompilerNode::ShaderCompilerNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mLastShaderLoad( 0 )
{
	FUGID( PIN_INPUT_VERTEX, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_TESSCTRL, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_INPUT_TESSEVAL, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );
	FUGID( PIN_INPUT_GEOMETRY, "249f2932-f483-422f-b811-ab679f006381" );
	FUGID( PIN_INPUT_FRAGMENT, "ce8d578e-c5a4-422f-b3c4-a1bdf40facdb" );

	FUGID( PIN_VARYINGS,	"de12f397-d3ba-4d2e-9f53-5da4ed4bff37" );
	FUGID( PIN_BUFFER_MODE,	"0A134F4F-4A33-4E74-98AF-F89AFC4FB19D" );

	mValInputVertex = pinInput<fugio::SyntaxErrorInterface *>( "Vertex", mPinShaderVertex, PID_SYNTAX_ERROR, PIN_INPUT_VERTEX );
	mValInputTessCtrl = pinInput<fugio::SyntaxErrorInterface *>( "Tess Ctrl", mPinShaderTessCtrl, PID_SYNTAX_ERROR, PIN_INPUT_TESSCTRL );
	mValInputTessEval = pinInput<fugio::SyntaxErrorInterface *>( "Tess Eval", mPinShaderTessEval, PID_SYNTAX_ERROR, PIN_INPUT_TESSEVAL );
	mValInputGeometry = pinInput<fugio::SyntaxErrorInterface *>( "Geometry", mPinShaderGeometry, PID_SYNTAX_ERROR, PIN_INPUT_GEOMETRY );
	mValInputFragment = pinInput<fugio::SyntaxErrorInterface *>( "Fragment", mPinShaderFragment, PID_SYNTAX_ERROR, PIN_INPUT_FRAGMENT );

	mPinInputVaryings = pinInput( "Varyings", PIN_VARYINGS );

	mValInputBufferMode = pinInput<fugio::ChoiceInterface *>( "Mode", mPinInputBufferMode, PID_CHOICE, PIN_BUFFER_MODE );

	QStringList		BufferModeChoices;

	BufferModeChoices << "GL_INTERLEAVED_ATTRIBS";
	BufferModeChoices << "GL_SEPARATE_ATTRIBS";

	mValInputBufferMode->setChoices( BufferModeChoices );

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
	mShaderCompilerData.clear();
}

bool ShaderCompilerNode::initialise()
{
	if( !fugio::NodeControlBase::initialise() )
	{
		return( false );
	}

	// set the syntax highlighting hint

	mPinShaderVertex->setSetting( PIN_SETTING_SYNTAX_HIGHLIGHTER, SYNTAX_HIGHLIGHTER_GLSL );
	mPinShaderTessCtrl->setSetting( PIN_SETTING_SYNTAX_HIGHLIGHTER, SYNTAX_HIGHLIGHTER_GLSL );
	mPinShaderTessEval->setSetting( PIN_SETTING_SYNTAX_HIGHLIGHTER, SYNTAX_HIGHLIGHTER_GLSL );
	mPinShaderGeometry->setSetting( PIN_SETTING_SYNTAX_HIGHLIGHTER, SYNTAX_HIGHLIGHTER_GLSL );
	mPinShaderFragment->setSetting( PIN_SETTING_SYNTAX_HIGHLIGHTER, SYNTAX_HIGHLIGHTER_GLSL );

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

void ShaderCompilerNode::loadShader( QSharedPointer<fugio::PinInterface> pPin, GLuint pProgramId, GLuint &pShaderId, GLenum pShaderType, int &pCompiled, int &pFailed )
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

						fugio::SyntaxErrorInterface *SyntaxErrors = ( pPin->hasControl() ? qobject_cast<fugio::SyntaxErrorInterface *>( pPin->control()->qobject() ) : nullptr );

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

							if( SyntaxErrors )
							{
								QList<fugio::SyntaxError>	SE;

								OpenGLPlugin::parseShaderErrors( QString( Log.data() ), SE );

								if( SE.isEmpty() )
								{
									SyntaxErrors->clearSyntaxErrors();
								}
								else
								{
									SyntaxErrors->setSyntaxErrors( SE );
								}
							}
							else
							{
								qWarning() << QString( Log.data() );
							}
						}

						glGetShaderiv( pShaderId, GL_COMPILE_STATUS, &Result );

						if( Result == GL_TRUE )
						{
							glAttachShader( pProgramId, pShaderId );

							if( SyntaxErrors )
							{
								 SyntaxErrors->clearSyntaxErrors();
							}

							pCompiled++;

							return;
						}

						//if( !QOpenGLContext::currentContext()->format().testOption( QSurfaceFormat::DebugContext ) )
						{
							//qWarning() << pPin->node()->name() << QString( Log.data() );
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
	ShaderCompilerData		CompilerData;

	GLuint					ShaderVertId = 0;
	GLuint					ShaderGeomId = 0;
	GLuint					ShaderTessCtrlId = 0;
	GLuint					ShaderTessEvalId = 0;
	GLuint					ShaderFragId = 0;

	GLint					Compiled = 0;
	GLint					Failed   = 0;

	//clearShader();

	if( ( CompilerData.mProgramId = glCreateProgram() ) == 0 )
	{
		return;
	}

	OPENGL_PLUGIN_DEBUG;

	loadShader( mPinShaderVertex, CompilerData.mProgramId, ShaderVertId, GL_VERTEX_SHADER, Compiled, Failed );

	OPENGL_PLUGIN_DEBUG;

#if defined( GL_TESS_CONTROL_SHADER )
	loadShader( mPinShaderTessCtrl, CompilerData.mProgramId, ShaderTessCtrlId, GL_TESS_CONTROL_SHADER, Compiled, Failed );

	OPENGL_PLUGIN_DEBUG;
#endif

#if defined( GL_TESS_EVALUATION_SHADER )
	loadShader( mPinShaderTessEval, CompilerData.mProgramId, ShaderTessEvalId, GL_TESS_EVALUATION_SHADER, Compiled, Failed );

	OPENGL_PLUGIN_DEBUG;
#endif

#if defined( GL_GEOMETRY_SHADER )
	loadShader( mPinShaderGeometry, CompilerData.mProgramId, ShaderGeomId, GL_GEOMETRY_SHADER, Compiled, Failed );
#endif

	OPENGL_PLUGIN_DEBUG;

	loadShader( mPinShaderFragment, CompilerData.mProgramId, ShaderFragId, GL_FRAGMENT_SHADER, Compiled, Failed );

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
			glTransformFeedbackVaryings( CompilerData.mProgramId, VarLst.size(), (const GLchar **)VarLst.constData(), BufMod );

			OPENGL_PLUGIN_DEBUG;
		}
	}
#endif

	//-------------------------------------------------------------------------
	// Link

	glLinkProgram( CompilerData.mProgramId );

	OPENGL_PLUGIN_DEBUG;

	GLint			LinkLogLength = 0;

	glGetProgramiv( CompilerData.mProgramId, GL_INFO_LOG_LENGTH, &LinkLogLength );

	if( LinkLogLength > 0 )
	{
		QVector<GLchar>	Log( LinkLogLength + 1 );

		glGetProgramInfoLog( CompilerData.mProgramId, LinkLogLength, &LinkLogLength, Log.data() );

		mNode->setStatusMessage( QString( Log.constData() ) );
	}

	OPENGL_PLUGIN_DEBUG;

	//-------------------------------------------------------------------------
	// we don't need to keep these around...

	if( ShaderFragId )
	{
		glDeleteShader( ShaderFragId );

		ShaderFragId = 0;
	}

	if( ShaderGeomId )
	{
		glDeleteShader( ShaderGeomId );

		ShaderGeomId = 0;
	}

	if( ShaderTessEvalId )
	{
		glDeleteShader( ShaderTessEvalId );

		ShaderTessEvalId = 0;
	}

	if( ShaderTessCtrlId )
	{
		glDeleteShader( ShaderTessCtrlId );

		ShaderTessCtrlId = 0;
	}

	if( ShaderVertId )
	{
		glDeleteShader( ShaderVertId );

		ShaderVertId = 0;
	}

	//-------------------------------------------------------------------------

	GLint			LinkStatus = GL_FALSE;

	glGetProgramiv( CompilerData.mProgramId, GL_LINK_STATUS, &LinkStatus );

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

	CompilerData.mProgramLinked = true;

	//-------------------------------------------------------------------------

	OPENGL_PLUGIN_DEBUG;

	CompilerData.process();

	OPENGL_PLUGIN_DEBUG;

	std::swap( mShaderCompilerData, CompilerData );

	CompilerData.clear();

	pinUpdated( mOutputPinShader );
}

void ShaderCompilerNode::ShaderCompilerData::process()
{
	ShaderUniformData	UniformData;

	memset( &UniformData, 0, sizeof( UniformData ) );

	mShaderUniformTypes.clear();
	mShaderAttributeTypes.clear();

	mUniformNames.clear();
	mAttributeNames.clear();

	GLint		TextureBinding = 0;

	GLint		ActiveUniforms = 0;

	glGetProgramiv( mProgramId, GL_ACTIVE_UNIFORMS, &ActiveUniforms );

	if( ActiveUniforms > 0 )
	{
		GLint		ActiveUniformsMaxLength;

		glGetProgramiv( mProgramId, GL_ACTIVE_UNIFORM_MAX_LENGTH, &ActiveUniformsMaxLength );

		std::vector<GLchar>		Name( ActiveUniformsMaxLength );

		for( GLint i = 0 ; i < ActiveUniforms ; i++ )
		{
			GLsizei		NameLength;

			glGetActiveUniform( mProgramId, i, Name.size(), &NameLength, &UniformData.mSize, &UniformData.mType, &Name[ 0 ] );

			QString		UniformName = QString::fromLocal8Bit( &Name[ 0 ] );

			UniformData.mLocation = glGetUniformLocation( mProgramId, Name.data() );

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
				case GL_INT_SAMPLER_2D:
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

	glGetProgramiv( mProgramId, GL_ACTIVE_ATTRIBUTES, &ActiveAtrributes );

	if( ActiveAtrributes > 0 )
	{
		GLint		ActiveUniformsMaxLength;

		glGetProgramiv( mProgramId, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &ActiveUniformsMaxLength );

		std::vector<GLchar>		Name( ActiveUniformsMaxLength );

		for( GLint i = 0 ; i < ActiveAtrributes ; i++ )
		{
			GLsizei		NameLength;

			glGetActiveAttrib( mProgramId, i, Name.size(), &NameLength, &UniformData.mSize, &UniformData.mType, &Name[ 0 ] );

			QString		AttributeName = QString::fromLocal8Bit( &Name[ 0 ] );

			UniformData.mLocation = glGetAttribLocation( mProgramId, Name.data() );

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

void ShaderCompilerNode::ShaderCompilerData::clear()
{
	if( mProgramId )
	{
		glDeleteProgram( mProgramId );

		mProgramId = 0;
	}

	mShaderAttributeTypes.clear();
	mShaderUniformTypes.clear();

	mUniformNames.clear();
	mAttributeNames.clear();

	mProgramLinked = false;
}
