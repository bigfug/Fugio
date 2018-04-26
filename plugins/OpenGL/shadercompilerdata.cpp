#include "shadercompilerdata.h"

#include <QOpenGLFunctions>

#if !defined( QT_OPENGL_ES_2 )
#include <QOpenGLFunctions_3_2_Core>
#endif

ShaderCompilerData::ShaderCompilerData()
	: mProgram( new QOpenGLShaderProgram() )
{

}

ShaderCompilerData::~ShaderCompilerData()
{
	clear();

	if( mProgram )
	{
		mProgram->deleteLater();
	}
}

ShaderCompilerData &ShaderCompilerData::operator =( ShaderCompilerData &&other )
{
	std::swap( mProgram, other.mProgram );

	if( other.mProgram )
	{
		other.mProgram->removeAllShaders();
	}

	mShaderUniformTypes   = std::move( other.mShaderUniformTypes );
	mShaderAttributeTypes = std::move( other.mShaderAttributeTypes );
	mUniformNames         = std::move( other.mUniformNames );
	mAttributeNames       = std::move( other.mAttributeNames );
	mFragmentOutputs      = std::move( other.mFragmentOutputs );

	return( *this );
}


void ShaderCompilerData::process()
{
	QOpenGLFunctions			*GL = QOpenGLContext::currentContext()->functions();

	fugio::ShaderUniformData	UniformData;

	memset( &UniformData, 0, sizeof( UniformData ) );

	mShaderUniformTypes.clear();
	mShaderAttributeTypes.clear();

	mUniformNames.clear();
	mAttributeNames.clear();
	mFragmentOutputs.clear();

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
#if !defined( QT_OPENGL_ES_2 )
				case GL_SAMPLER_1D:
				case GL_SAMPLER_1D_ARRAY:
				case GL_SAMPLER_1D_ARRAY_SHADOW:
				case GL_SAMPLER_1D_SHADOW:
				case GL_SAMPLER_2D_RECT:
				case GL_SAMPLER_CUBE_MAP_ARRAY:
				case GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW:
				case GL_INT_SAMPLER_1D:
				case GL_INT_SAMPLER_1D_ARRAY:
				case GL_UNSIGNED_INT_SAMPLER_1D:
				case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
				case GL_SAMPLER_2D_RECT_SHADOW:
				case GL_SAMPLER_BUFFER:
				case GL_INT_SAMPLER_2D_RECT:
				case GL_INT_SAMPLER_BUFFER:
				case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
				case GL_UNSIGNED_INT_SAMPLER_BUFFER:
				case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
				case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
				case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
				case GL_INT_SAMPLER_CUBE_MAP_ARRAY:
				case GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY:
#endif
				case GL_SAMPLER_2D_ARRAY:
				case GL_SAMPLER_2D_ARRAY_SHADOW:
				case GL_SAMPLER_2D_MULTISAMPLE:
				case GL_SAMPLER_2D_SHADOW:
				case GL_SAMPLER_3D:
				case GL_SAMPLER_2D:
				case GL_SAMPLER_CUBE:
				case GL_SAMPLER_CUBE_SHADOW:
				case GL_INT_SAMPLER_2D:
				case GL_INT_SAMPLER_3D:
				case GL_INT_SAMPLER_CUBE:
				case GL_INT_SAMPLER_2D_ARRAY:
				case GL_UNSIGNED_INT_SAMPLER_2D:
				case GL_UNSIGNED_INT_SAMPLER_3D:
				case GL_UNSIGNED_INT_SAMPLER_CUBE:
				case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
				case GL_INT_SAMPLER_2D_MULTISAMPLE:
				case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
				case GL_SAMPLER:
					UniformData.mSampler = true;
					UniformData.mTextureBinding = TextureBinding++;

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

#if !defined( QT_OPENGL_ES_2 )
	QOpenGLFunctions_3_2_Core		*GL32 = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_2_Core>();

	if( GL32 && GL32->initializeOpenGLFunctions() )
	{
		GLint		MaxDrawBuffers = 0;

		GL32->glGetIntegerv( GL_MAX_DRAW_BUFFERS, &MaxDrawBuffers );

		mFragmentOutputs.resize( MaxDrawBuffers );

		for( int i = 0 ; i < MaxDrawBuffers ; i++ )
		{
			const QString	FragName = QString( "fug_Fragment%1" ).arg( i );

			GLint	FragData = GL32->glGetFragDataLocation( mProgram->programId(), FragName.toLatin1().constData() );

			mFragmentOutputs[ i ] = ( FragData == -1 ? GL_NONE : GL_COLOR_ATTACHMENT0 + i );
		}

		while( !mFragmentOutputs.isEmpty() && mFragmentOutputs.last() == GL_NONE )
		{
			mFragmentOutputs.removeLast();
		}
	}
#endif

	//emit updateUniformList( mUniformNames );
}

void ShaderCompilerData::clear()
{
	if( mProgram )
	{
		mProgram->removeAllShaders();
	}

	mShaderAttributeTypes.clear();
	mShaderUniformTypes.clear();

	mUniformNames.clear();
	mAttributeNames.clear();
	mFragmentOutputs.clear();
}
