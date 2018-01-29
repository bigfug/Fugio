#include "shadercompilernode.h"

#if QT_VERSION >= QT_VERSION_CHECK( 5, 6, 0 )
#include <QOpenGLExtraFunctions>
#endif

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

#if defined( QOPENGLEXTRAFUNCTIONS_H )
	QOpenGLExtraFunctions	*GLEX = QOpenGLContext::currentContext()->extraFunctions();
#endif

	OPENGL_PLUGIN_DEBUG;

	OpenGLPlugin::loadShader( mPinShaderVertex, *CompilerData.mProgram, QOpenGLShader::Vertex, Compiled, Failed );

	OPENGL_PLUGIN_DEBUG;

	OpenGLPlugin::loadShader( mPinShaderTessCtrl, *CompilerData.mProgram, QOpenGLShader::TessellationControl, Compiled, Failed );

	OPENGL_PLUGIN_DEBUG;

	OpenGLPlugin::loadShader( mPinShaderTessEval, *CompilerData.mProgram, QOpenGLShader::TessellationEvaluation, Compiled, Failed );

	OPENGL_PLUGIN_DEBUG;

	OpenGLPlugin::loadShader( mPinShaderGeometry, *CompilerData.mProgram, QOpenGLShader::Geometry, Compiled, Failed );

	OPENGL_PLUGIN_DEBUG;

	OpenGLPlugin::loadShader( mPinShaderFragment, *CompilerData.mProgram, QOpenGLShader::Fragment, Compiled, Failed );

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

#if defined( QOPENGLEXTRAFUNCTIONS_H )
		if( GLEX )
		{
			GLEX->glTransformFeedbackVaryings( CompilerData.mProgram->programId(), VarLst.size(), (const GLchar **)VarLst.constData(), BufMod );
		}
#endif

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
