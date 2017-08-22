#include "shadercompilernode.h"

#include <QOpenGLExtraFunctions>

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
	FUGID( PIN_OUTPUT_SHADER, "e6bf944e-5f46-4994-bd51-13c2aa6415b7" );

	FUGID( PIN_VARYINGS,	"de12f397-d3ba-4d2e-9f53-5da4ed4bff37" );
	FUGID( PIN_BUFFER_MODE,	"0A134F4F-4A33-4E74-98AF-F89AFC4FB19D" );

	mValInputVertex   = pinInput<fugio::SyntaxErrorInterface *>( "Vertex", mPinShaderVertex, PID_SYNTAX_ERROR, PIN_INPUT_VERTEX );
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

	mOutputShader = pinOutput<OpenGLShaderInterface *>( "Shader", mOutputPinShader, PID_OPENGL_SHADER, PIN_OUTPUT_SHADER );

	qobject_cast<ShaderPin *>( mOutputPinShader->control()->qobject() )->setParent( this );

	mPinShaderVertex->setDescription( tr( "The source code for an OpenGL vertex shader - use a Text Editor node, or load from a file" ) );
	mPinShaderTessCtrl->setDescription( tr( "The source code for an OpenGL tessellation control shader - use a Text Editor node, or load from a file" ) );
	mPinShaderTessEval->setDescription( tr( "The source code for an OpenGL tessellation evaluation shader - use a Text Editor node, or load from a file" ) );
	mPinShaderGeometry->setDescription( tr( "The source code for an OpenGL geometry shader - use a Text Editor node, or load from a file" ) );
	mPinShaderFragment->setDescription( tr( "The source code for an OpenGL fragment shader - use a Text Editor node, or load from a file" ) );

	mOutputPinShader->setDescription( tr( "The compiled shader made from all of the source code inputs" ) );

	// set the syntax highlighting hint

	mValInputVertex->setHighlighterUuid( SYNTAX_HIGHLIGHTER_GLSL );
	mValInputTessCtrl->setHighlighterUuid( SYNTAX_HIGHLIGHTER_GLSL );
	mValInputTessEval->setHighlighterUuid( SYNTAX_HIGHLIGHTER_GLSL );
	mValInputGeometry->setHighlighterUuid( SYNTAX_HIGHLIGHTER_GLSL );
	mValInputFragment->setHighlighterUuid( SYNTAX_HIGHLIGHTER_GLSL );
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
	if( !pTimeStamp )
	{
		return;
	}

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

void ShaderCompilerNode::loadShader( QSharedPointer<fugio::PinInterface> pPin, QOpenGLShaderProgram &pProgram, QOpenGLShader::ShaderType pShaderType, int &pCompiled, int &pFailed )
{
	fugio::SyntaxErrorInterface *SyntaxErrors = input<fugio::SyntaxErrorInterface *>( pPin );

	QString			Source = variant( pPin ).toString();

	if( Source.isEmpty() )
	{
		if( SyntaxErrors )
		{
			SyntaxErrors->clearSyntaxErrors();
		}

		return;
	}

	if( !pProgram.addShaderFromSourceCode( pShaderType, Source ) )
	{
		QString		Log = pProgram.log();

		if( !Log.isEmpty() )
		{
			qWarning() << Log;
		}

		if( SyntaxErrors )
		{
			QList<fugio::SyntaxError>	SE;

			OpenGLPlugin::parseShaderErrors( Log, SE );

			if( SE.isEmpty() )
			{
				SyntaxErrors->clearSyntaxErrors();
			}
			else
			{
				SyntaxErrors->setSyntaxErrors( SE );
			}
		}

		pFailed++;

		return;
	}

	pCompiled++;
}

void ShaderCompilerNode::loadShader()
{
	ShaderCompilerData		CompilerData;

	GLint					Compiled = 0;
	GLint					Failed   = 0;

	//clearShader();

	if( !CompilerData.mProgram->create() )
	{
		return;
	}

	QOpenGLExtraFunctions	*GLEX = QOpenGLContext::currentContext()->extraFunctions();

	OPENGL_PLUGIN_DEBUG;

	loadShader( mPinShaderVertex, *CompilerData.mProgram, QOpenGLShader::Vertex, Compiled, Failed );

	OPENGL_PLUGIN_DEBUG;

	loadShader( mPinShaderTessCtrl, *CompilerData.mProgram, QOpenGLShader::TessellationControl, Compiled, Failed );

	OPENGL_PLUGIN_DEBUG;

	loadShader( mPinShaderTessEval, *CompilerData.mProgram, QOpenGLShader::TessellationEvaluation, Compiled, Failed );

	OPENGL_PLUGIN_DEBUG;

	loadShader( mPinShaderGeometry, *CompilerData.mProgram, QOpenGLShader::Geometry, Compiled, Failed );

	OPENGL_PLUGIN_DEBUG;

	loadShader( mPinShaderFragment, *CompilerData.mProgram, QOpenGLShader::Fragment, Compiled, Failed );

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

		if( GLEX )
		{
			GLEX->glTransformFeedbackVaryings( CompilerData.mProgram->programId(), VarLst.size(), (const GLchar **)VarLst.constData(), BufMod );
		}

		OPENGL_PLUGIN_DEBUG;
	}
#endif

	//-------------------------------------------------------------------------
	// Link

	bool	LinkResult = CompilerData.mProgram->link();

	mNode->setStatusMessage( CompilerData.mProgram->log() );

	if( !LinkResult )
	{
		mNode->setStatus( fugio::NodeInterface::Error );

		return;
	}

	mNode->setStatus( fugio::NodeInterface::Initialised );

	mNode->setStatusMessage( "Shader Linked" );

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
	QOpenGLFunctions	*GL = QOpenGLContext::currentContext()->functions();

	ShaderUniformData	UniformData;

	memset( &UniformData, 0, sizeof( UniformData ) );

	mShaderUniformTypes.clear();
	mShaderAttributeTypes.clear();

	mUniformNames.clear();
	mAttributeNames.clear();

	GLint		TextureBinding = 0;

	GLint		ActiveUniforms = 0;

	GL->glGetProgramiv( mProgram->programId(), GL_ACTIVE_UNIFORMS, &ActiveUniforms );

	if( ActiveUniforms > 0 )
	{
		GLint		ActiveUniformsMaxLength;

		GL->glGetProgramiv( mProgram->programId(), GL_ACTIVE_UNIFORM_MAX_LENGTH, &ActiveUniformsMaxLength );

		std::vector<GLchar>		Name( ActiveUniformsMaxLength );

		for( GLint i = 0 ; i < ActiveUniforms ; i++ )
		{
			GLsizei		NameLength;

			GL->glGetActiveUniform( mProgram->programId(), i, Name.size(), &NameLength, &UniformData.mSize, &UniformData.mType, &Name[ 0 ] );

			QString		UniformName = QString::fromLocal8Bit( &Name[ 0 ] );

			UniformData.mLocation = mProgram->uniformLocation( Name.data() );

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
				case GL_SAMPLER_2D:
				case GL_SAMPLER_CUBE:
				case GL_SAMPLER_CUBE_MAP_ARRAY:
				case GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW:
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

	GL->glGetProgramiv( mProgram->programId(), GL_ACTIVE_ATTRIBUTES, &ActiveAtrributes );

	if( ActiveAtrributes > 0 )
	{
		GLint		ActiveUniformsMaxLength;

		GL->glGetProgramiv( mProgram->programId(), GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &ActiveUniformsMaxLength );

		std::vector<GLchar>		Name( ActiveUniformsMaxLength );

		for( GLint i = 0 ; i < ActiveAtrributes ; i++ )
		{
			GLsizei		NameLength;

			GL->glGetActiveAttrib( mProgram->programId(), i, Name.size(), &NameLength, &UniformData.mSize, &UniformData.mType, &Name[ 0 ] );

			QString		AttributeName = QString::fromLocal8Bit( &Name[ 0 ] );

			UniformData.mLocation = mProgram->attributeLocation( Name.data() );

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
	if( mProgram )
	{
		mProgram->removeAllShaders();
	}

	mShaderAttributeTypes.clear();
	mShaderUniformTypes.clear();

	mUniformNames.clear();
	mAttributeNames.clear();
}
