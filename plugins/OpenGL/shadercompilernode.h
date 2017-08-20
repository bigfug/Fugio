#ifndef SHADERCOMPILER_H
#define SHADERCOMPILER_H

#include "opengl_includes.h"

#include <QObject>
#include <QPointer>

#include <fugio/nodecontrolbase.h>
#include <fugio/opengl/shader_interface.h>
#include <fugio/text/syntax_highlighter_instance_interface.h>
#include <fugio/choice_interface.h>
#include <fugio/opengl/vertex_array_object_interface.h>

#include "vertexarrayobjectpin.h"

#include <fugio/text/syntax_error_interface.h>

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
		return( mShaderCompilerData.mProgramLinked );
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
		return( mShaderCompilerData.mProgramId );
	}

private:
	void loadShader( QSharedPointer<fugio::PinInterface> pPin, GLuint pProgramId, GLuint &pShaderId, GLenum pShaderType, int &pCompiled, int &pFailed );

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

	typedef struct ShaderCompilerData
	{
		ShaderUniformMap						 mShaderUniformTypes;
		ShaderUniformMap						 mShaderAttributeTypes;

		QStringList								 mUniformNames;
		QStringList								 mAttributeNames;

		GLuint									 mProgramId;
		GLboolean								 mProgramLinked;

		ShaderCompilerData( void )
			: mProgramId( 0 ), mProgramLinked( false )
		{

		}

		ShaderCompilerData( ShaderCompilerData &&other )
		{
			*this = std::move( other );
		}

		~ShaderCompilerData( void )
		{
			clear();
		}

		ShaderCompilerData &operator = ( ShaderCompilerData && other )
		{
			mProgramId = other.mProgramId;
			mProgramLinked = other.mProgramLinked;

			other.mProgramId = 0;
			other.mProgramLinked = false;

			mShaderUniformTypes = std::move( other.mShaderUniformTypes );
			mShaderAttributeTypes = std::move( other.mShaderAttributeTypes );
			mUniformNames = std::move( other.mUniformNames );
			mAttributeNames = std::move( other.mAttributeNames );

			return( *this );
		}

		void process( void );

		void clear( void );

	} ShaderCompilerData;

	ShaderCompilerData						 mShaderCompilerData;
};

#endif // SHADERCOMPILER_H
