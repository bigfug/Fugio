#ifndef INTERFACE_OPENGL_SHADER_H
#define INTERFACE_OPENGL_SHADER_H

#include <fugio/global.h>

#include <QMap>

class QOpenGLShaderProgram;

FUGIO_NAMESPACE_BEGIN

class OpenGLTextureInterface;

typedef struct ShaderUniformData
{
	GLenum							 mType;
	GLint							 mSize;
	GLint							 mTextureBinding;
	GLint							 mLocation;
	GLboolean						 mSampler;
} ShaderUniformData;

typedef struct ShaderBindData
{
	GLenum							 mUnit;
	OpenGLTextureInterface			*mTexture;
} ShaderBindData;

typedef QMap<QString, ShaderUniformData>	 ShaderUniformMap;

class OpenGLShaderInterface
{
public:
	virtual ~OpenGLShaderInterface( void ) {}

	virtual bool isLinked( void ) const = 0;

	virtual const fugio::ShaderUniformMap &uniformMap( void ) const = 0;

	virtual const fugio::ShaderUniformMap &attributeMap( void ) const = 0;

	virtual GLuint programId( void ) const = 0;

	virtual QOpenGLShaderProgram *program( void ) = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::OpenGLShaderInterface, "com.bigfug.fugio.opengl.shader/1.0" )

#endif // INTERFACE_OPENGL_SHADER_H

