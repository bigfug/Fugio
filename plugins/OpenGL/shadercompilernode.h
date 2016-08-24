#ifndef SHADERCOMPILER_H
#define SHADERCOMPILER_H

#include "opengl_includes.h"

#include <QObject>
#include <QPointer>

#include <fugio/nodecontrolbase.h>
#include <fugio/opengl/shader_interface.h>
#include <fugio/text/syntax_highlighter_interface.h>
#include <fugio/choice_interface.h>
#include <fugio/opengl/vertex_array_object_interface.h>

#include "vertexarrayobjectpin.h"

#include "syntaxhighlighterglsl.h"

using namespace fugio;

class ShaderHighlighter : public QObject, public fugio::SyntaxHighlighterInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::SyntaxHighlighterInterface )

public:
	ShaderHighlighter( QObject *pParent = 0 )
		: QObject( pParent ), mHighlighter( 0 )
	{

	}

	void clearErrors( void );

	void setErrors( const QString &pErrorText );

	//-------------------------------------------------------------------------
	// fugio::InterfaceSyntaxHighlighter

	virtual QSyntaxHighlighter *highlighter( QTextDocument *pDocument ) Q_DECL_OVERRIDE;

	virtual QStringList errorList( int pLineNumber ) const Q_DECL_OVERRIDE
	{
		return( mHighlighter ? mHighlighter->errorList( pLineNumber ) : QStringList() );
	}

private:
	QPointer<SyntaxHighlighterGLSL>				mHighlighter;
};

class ShaderCompilerNode : public fugio::NodeControlBase, public fugio::OpenGLShaderInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::OpenGLShaderInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Compiles OpenGL shader code into a reusable shader" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/OpenGL_Shader_Compiler" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE ShaderCompilerNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~ShaderCompilerNode( void );

	//-------------------------------------------------------------------------
	// NodeControlInterface interface

	virtual bool initialise( void );

	virtual bool deinitialise( void );

	virtual void inputsUpdated( qint64 pTimeStamp );

	//-------------------------------------------------------------------------
	// 	InterfaceOpenGLShader

	virtual bool isLinked( void ) const
	{
		return( mProgramLinked );
	}

	virtual const fugio::ShaderUniformMap &uniformMap( void ) const
	{
		return( mShaderUniformTypes );
	}

	virtual const fugio::ShaderUniformMap &attributeMap( void ) const
	{
		return( mShaderAttributeTypes );
	}

	virtual GLuint programId( void ) const
	{
		return( mProgramId );
	}

private:
	void loadShader( QSharedPointer<fugio::PinInterface> pPin, GLuint &pShaderId, GLenum pShaderType, int &pCompiled, int &pFailed );
	void loadShader();
	void clearShader();
	void processShader(GLuint pProgramId);

protected:
	QSharedPointer<fugio::PinInterface>		 mPinInputVaryings;
	QSharedPointer<fugio::PinInterface>		 mPinShaderVertex;
	QSharedPointer<fugio::PinInterface>		 mPinShaderGeometry;
	QSharedPointer<fugio::PinInterface>		 mPinShaderTessCtrl;
	QSharedPointer<fugio::PinInterface>		 mPinShaderTessEval;
	QSharedPointer<fugio::PinInterface>		 mPinShaderFragment;
	QSharedPointer<fugio::PinInterface>		 mPinInputBufferMode;

	fugio::ChoiceInterface					*mValInputBufferMode;

	QSharedPointer<fugio::PinInterface>		 mOutputPinShader;
	OpenGLShaderInterface					*mOutputShader;

	ShaderUniformMap						 mShaderUniformTypes;
	ShaderUniformMap						 mShaderAttributeTypes;

	QStringList								 mUniformNames;
	QStringList								 mAttributeNames;

	GLuint									 mProgramId;
	GLboolean								 mProgramLinked;
	qint64									 mLastShaderLoad;
	GLuint									 mShaderVertId;
	GLuint									 mShaderGeomId;
	GLuint									 mShaderTessCtrlId;
	GLuint									 mShaderTessEvalId;
	GLuint									 mShaderFragId;
};

#endif // SHADERCOMPILER_H
