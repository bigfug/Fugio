#ifndef SHADERCOMPILERDATA_H
#define SHADERCOMPILERDATA_H

#include <QOpenGLShaderProgram>
#include <QStringList>

#include <fugio/opengl/shader_interface.h>

typedef struct ShaderCompilerData
{
	fugio::ShaderUniformMap					 mShaderUniformTypes;
	fugio::ShaderUniformMap					 mShaderAttributeTypes;

	QStringList								 mUniformNames;
	QStringList								 mAttributeNames;
	QVector<GLenum>							 mFragmentOutputs;

	QOpenGLShaderProgram					*mProgram;

	ShaderCompilerData( void );

	ShaderCompilerData( ShaderCompilerData &&other )
		: mProgram( Q_NULLPTR ) //new QOpenGLShaderProgram() )
	{
		*this = std::move( other );
	}

	~ShaderCompilerData( void );

	ShaderCompilerData &operator = ( ShaderCompilerData && other );

	void process( void );

	void clear( void );

private:
	Q_DISABLE_COPY( ShaderCompilerData )

  } ShaderCompilerData;


#endif // SHADERCOMPILERDATA_H
