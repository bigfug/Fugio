#ifndef SHADERCOMPILER_H
#define SHADERCOMPILER_H

#include "opengl_includes.h"

#include <QObject>
#include <QPointer>
#include <QOpenGLShaderProgram>

#include <fugio/nodecontrolbase.h>
#include <fugio/opengl/shader_interface.h>
#include <fugio/text/syntax_highlighter_instance_interface.h>
#include <fugio/choice_interface.h>
#include <fugio/opengl/vertex_array_object_interface.h>

#include <fugio/text/syntax_error_interface.h>

#include "shadercompilerdata.h"

using namespace fugio;

class ShaderCompilerNode : public fugio::NodeControlBase, public fugio::OpenGLShaderInterface, protected QOpenGLFunctions
{
	Q_OBJECT
	Q_INTERFACES( fugio::OpenGLShaderInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Compiles OpenGL shader code into a reusable shader" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Shader_Compiler_(OpenGL)" ) )
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
		return( mShaderCompilerData.mProgram->isLinked() );
	}

	virtual const fugio::ShaderUniformMap &uniformMap( void ) const
	{
		return( mShaderCompilerData.mShaderUniformTypes );
	}

	virtual const fugio::ShaderUniformMap &attributeMap( void ) const
	{
		return( mShaderCompilerData.mShaderAttributeTypes );
	}

	virtual GLuint programId( void ) const
	{
		return( mShaderCompilerData.mProgram->programId() );
	}

	virtual QOpenGLShaderProgram *program( void )
	{
		return( mShaderCompilerData.mProgram );
	}

private:
	void loadShader();

protected:
	QSharedPointer<fugio::PinInterface>		 mPinInputVaryings;
	QSharedPointer<fugio::PinInterface>		 mPinShaderVertex;
	QSharedPointer<fugio::PinInterface>		 mPinShaderGeometry;
	QSharedPointer<fugio::PinInterface>		 mPinShaderTessCtrl;
	QSharedPointer<fugio::PinInterface>		 mPinShaderTessEval;
	QSharedPointer<fugio::PinInterface>		 mPinShaderFragment;
	QSharedPointer<fugio::PinInterface>		 mPinInputBufferMode;

	fugio::SyntaxErrorInterface				*mValInputVertex;
	fugio::SyntaxErrorInterface				*mValInputTessCtrl;
	fugio::SyntaxErrorInterface				*mValInputTessEval;
	fugio::SyntaxErrorInterface				*mValInputGeometry;
	fugio::SyntaxErrorInterface				*mValInputFragment;

	fugio::ChoiceInterface					*mValInputBufferMode;

	QSharedPointer<fugio::PinInterface>		 mOutputPinShader;
	OpenGLShaderInterface					*mOutputShader;

	qint64									 mLastShaderLoad;

	ShaderCompilerData						 mShaderCompilerData;
};

#endif // SHADERCOMPILER_H
