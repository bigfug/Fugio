#include "easyshader2dnode.h"

#include <QOpenGLContext>

#if QT_VERSION >= QT_VERSION_CHECK( 5, 6, 0 )
#include <QOpenGLExtraFunctions>
#endif

#include <fugio/core/uuid.h>
#include <fugio/text/uuid.h>
#include <fugio/opengl/uuid.h>
#include <fugio/performance.h>
#include <fugio/opengl/texture_interface.h>
#include <fugio/core/array_interface.h>

#include "openglplugin.h"

EasyShader2DNode::EasyShader2DNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mQuadGeometry( QOpenGLBuffer::VertexBuffer )
{
	FUGID( PIN_INPUT_VERTEX_SHADER, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_FRAGMENT_SHADER, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_OUTPUT_RENDER, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );
	FUGID( PIN_OUTPUT_TEXTURE, "249f2932-f483-422f-b811-ab679f006381" );

	mPinInputTrigger = pinInput( "Trigger", PID_FUGIO_NODE_TRIGGER );

	mValInputShaderVertex = pinInput<fugio::SyntaxErrorInterface *>( "Vertex", mPinInputShaderVertex, PID_SYNTAX_ERROR, PIN_INPUT_VERTEX_SHADER );

	mValInputShaderFragment = pinInput<fugio::SyntaxErrorInterface *>( "Fragment", mPinInputShaderFragment, PID_SYNTAX_ERROR, PIN_INPUT_FRAGMENT_SHADER );

	mPinInputShaderVertex->registerPinInputType( PID_STRING );
	mPinInputShaderFragment->registerPinInputType( PID_STRING );

	mValInputShaderVertex->setHighlighterUuid( SYNTAX_HIGHLIGHTER_GLSL );
	mValInputShaderFragment->setHighlighterUuid( SYNTAX_HIGHLIGHTER_GLSL );

	mValOutputRender = pinOutput<fugio::RenderInterface *>( "Render", mPinOutputRender, PID_RENDER, PIN_OUTPUT_RENDER );

	pinOutput( "Texture0", PIN_OUTPUT_TEXTURE );
}

bool EasyShader2DNode::initialise( void )
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	if( !QOpenGLContext::currentContext() )
	{
		return( false );
	}

	if( mPinInputShaderVertex->value().toString().isEmpty() )
	{
		if( !QOpenGLContext::currentContext()->isOpenGLES() && QOpenGLContext::currentContext()->format().majorVersion() >= 3 )
		{
			const char *vertexSource =
					"#version 330\n\n"
					"in vec2 vertex;\n\n"
					"out vec2 TextureCoords;\n\n"
					"void main()\n"
					"{\n"
					"	gl_Position = vec4( vertex, 0.0, 1.0 );\n"
					"	TextureCoords = ( vertex * 0.5 ) + 0.5;\n"
					"}\n";

			mPinInputShaderVertex->setValue( vertexSource );
		}
		else
		{
			mPinInputShaderVertex->setValue(
				"attribute highp vec2 vertex;\n\n"
				"varying highp vec2 TextureCoords;\n\n"
				"void main( void )\n"
				"{\n"
				"	gl_Position = vec4( vertex, 0, 1 );\n"
				"	TextureCoords = ( vertex * 0.5 ) + 0.5;\n"
				"}\n" );
		}
	}

	if( mPinInputShaderFragment->value().toString().isEmpty() )
	{
		if( !QOpenGLContext::currentContext()->isOpenGLES() && QOpenGLContext::currentContext()->format().majorVersion() >= 3 )
		{
			const char *fragmentSource =
					"#version 330\n"
					"in vec2 TextureCoords;\n"
					"out vec4 outColor;\n\n"
					"void main()\n"
					"{\n"
					"	outColor = vec4( TextureCoords, 0.5, 1 );\n"
					"}\n";

			mPinInputShaderFragment->setValue( fragmentSource );
		}
		else
		{
			mPinInputShaderFragment->setValue(
				"varying highp vec2 TextureCoords;\n\n"
				"void main( void )\n"
				"{\n"
				"	gl_FragColor = vec4( TextureCoords, 0.5, 1 );\n"
				"}\n" );
		}
	}

	return( true );
}

bool EasyShader2DNode::deinitialise()
{
	mShaderCompilerData.clear();

	return( NodeControlBase::deinitialise() );
}

void EasyShader2DNode::inputsUpdated( qint64 pTimeStamp )
{
	if( mPinInputShaderVertex->isUpdated( pTimeStamp ) || mPinInputShaderFragment->isUpdated( pTimeStamp ) )
	{
		compileShader();

		createInputPins();

		createOutputPins();
	}

	if( !mShaderCompilerData.mProgram )
	{
		return;
	}

	updateInputPins();

	pinUpdated( mPinOutputRender );

	if( QOpenGLContext::currentContext() )
	{
		bool	Rendered = false;
		GLint	FboCur;

		glGetIntegerv( GL_FRAMEBUFFER_BINDING, &FboCur );

		GLint		VP[ 4 ];

		glGetIntegerv( GL_VIEWPORT, VP );

		for( QSharedPointer<fugio::PinInterface> P : mNode->enumOutputPins() )
		{
			if( P == mPinOutputRender )
			{
				continue;
			}

			if( !Rendered && P != mPinOutputRender && P->isConnected() )
			{
				updateOutputPins();

				if( mFBO.fboConst() )
				{
					render( pTimeStamp, QUuid() );

					Rendered = true;
				}
			}

			if( Rendered )
			{
				fugio::OpenGLTextureInterface	*TexInf = output<fugio::OpenGLTextureInterface *>( P );

				if( TexInf )
				{
					TexInf->swapTexture();
				}

				pinUpdated( P );
			}
		}

		glViewport( VP[ 0 ], VP[ 1 ], VP[ 2 ], VP[ 3 ] );

		glBindFramebuffer( GL_FRAMEBUFFER, FboCur );
	}
}

void EasyShader2DNode::render( qint64 pTimeStamp, QUuid pSourcePinId )
{
	Q_UNUSED( pSourcePinId )

	fugio::Performance	Perf( mNode, "render", pTimeStamp );

	if( !mShaderCompilerData.mProgram )
	{
		return;
	}

	initializeOpenGLFunctions();

	if( !mShaderCompilerData.mProgram->bind() )
	{
		return;
	}

	for( QSharedPointer<fugio::PinInterface> P : mNode->enumInputPins() )
	{
		if( P == mPinInputTrigger || P == mPinInputShaderVertex || P == mPinInputShaderFragment )
		{
			continue;
		}

		int		UniIdx = mShaderCompilerData.mUniformNames.indexOf( P->name() );

		if( UniIdx < 0 )
		{
			continue;
		}

		ShaderUniformData	UniDat = mShaderCompilerData.mShaderUniformTypes.value( P->name() );

		if( UniDat.mSampler )
		{
			fugio::OpenGLTextureInterface	*TexInf = input<fugio::OpenGLTextureInterface *>( P );

			if( TexInf )
			{
				glActiveTexture( GL_TEXTURE0 + UniDat.mTextureBinding );

				TexInf->srcBind();
			}
		}
	}

	GLint	VertexLocation = mShaderCompilerData.mProgram->attributeLocation( "vertex" );

	QOpenGLVertexArrayObject	*VAO = mVAO.vao();

	if( VAO && !VAO->isCreated() )
	{
		VAO->create();
		VAO->bind();

		static const GLfloat Verticies[] =
		{
			-1, -1,
			-1,  1,
			 1, -1,
			 1,  1
		};

		mQuadGeometry.create();
		mQuadGeometry.bind();
		mQuadGeometry.allocate( Verticies, sizeof( Verticies ) );

		mShaderCompilerData.mProgram->setAttributeBuffer( VertexLocation, GL_FLOAT, 0, 2 );
		mShaderCompilerData.mProgram->enableAttributeArray( VertexLocation );

		mQuadGeometry.release();

		VAO->release();
	}

	QOpenGLVertexArrayObject::Binder VAOBinder( VAO );

	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

	mShaderCompilerData.mProgram->release();

	for( QSharedPointer<fugio::PinInterface> P : mNode->enumInputPins() )
	{
		if( P == mPinInputTrigger || P == mPinInputShaderVertex || P == mPinInputShaderFragment )
		{
			continue;
		}

		int		UniIdx = mShaderCompilerData.mUniformNames.indexOf( P->name() );

		if( UniIdx < 0 )
		{
			continue;
		}

		ShaderUniformData	UniDat = mShaderCompilerData.mShaderUniformTypes.value( P->name() );

		if( UniDat.mSampler )
		{
			fugio::OpenGLTextureInterface	*TexInf = input<fugio::OpenGLTextureInterface *>( P );

			if( TexInf )
			{
				glActiveTexture( GL_TEXTURE0 + UniDat.mTextureBinding );

				TexInf->release();
			}
		}
	}

	glActiveTexture( GL_TEXTURE0 );
}

void EasyShader2DNode::compileShader()
{
	ShaderCompilerData	NewPrg;

	GLint				Compiled = 0;
	GLint				Failed   = 0;

	if( !NewPrg.mProgram->create() )
	{
		return;
	}

	OpenGLPlugin::loadShader( mPinInputShaderVertex, *NewPrg.mProgram, QOpenGLShader::Vertex, Compiled, Failed );

	OpenGLPlugin::loadShader( mPinInputShaderFragment, *NewPrg.mProgram, QOpenGLShader::Fragment, Compiled, Failed );

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

	//-------------------------------------------------------------------------
	// Link

	bool	LinkResult = NewPrg.mProgram->link();

	mNode->setStatusMessage( NewPrg.mProgram->log() );

	if( !LinkResult )
	{
		mNode->setStatus( fugio::NodeInterface::Error );

		return;
	}

	mNode->setStatus( fugio::NodeInterface::Initialised );

	mNode->setStatusMessage( "Shader Linked" );

	//-------------------------------------------------------------------------

	OPENGL_PLUGIN_DEBUG;

	NewPrg.process();

	OPENGL_PLUGIN_DEBUG;

	std::swap( mShaderCompilerData, NewPrg );

	NewPrg.clear();
}

void EasyShader2DNode::createInputPins()
{
	QStringList		UniformNames = mShaderCompilerData.mUniformNames;

	for( QSharedPointer<fugio::PinInterface> P : mNode->enumInputPins() )
	{
		if( P == mPinInputTrigger || P == mPinInputShaderVertex || P == mPinInputShaderFragment )
		{
			continue;
		}

		if( !UniformNames.contains( P->name() ) )
		{
			if( !P->isConnected() )
			{
				mNode->removePin( P );
			}
		}
		else
		{
			UniformNames.removeAll( P->name() );
		}
	}

	for( QString S : UniformNames )
	{
		QSharedPointer<fugio::PinInterface>		NewPin = pinInput( S, QUuid::createUuid() );

		if( NewPin )
		{

		}
	}
}

void EasyShader2DNode::createOutputPins()
{
}

void EasyShader2DNode::updateInputPins()
{
	initializeOpenGLFunctions();

	if( !mShaderCompilerData.mProgram->bind() )
	{
		return;
	}

	for( QSharedPointer<fugio::PinInterface> P : mNode->enumInputPins() )
	{
		if( P == mPinInputTrigger || P == mPinInputShaderVertex || P == mPinInputShaderFragment )
		{
			continue;
		}

		int		UniIdx = mShaderCompilerData.mUniformNames.indexOf( P->name() );

		if( UniIdx < 0 )
		{
			continue;
		}

		ShaderUniformData	UniDat = mShaderCompilerData.mShaderUniformTypes.value( P->name() );

		if( UniDat.mSampler )
		{
			mShaderCompilerData.mProgram->setUniformValue( UniDat.mLocation, UniDat.mTextureBinding );

			continue;
		}

		if( UniDat.mSize > 1 )
		{
			fugio::ArrayInterface			*ArrInt = input<fugio::ArrayInterface *>( P );

			if( ArrInt )
			{
				int			CpyCnt = qMin( ArrInt->count(), UniDat.mSize );

				if( !CpyCnt )
				{
					continue;
				}

				const void *ArrDat = ArrInt->array();

				if( !ArrDat )
				{
					continue;
				}

				switch( UniDat.mType )
				{
					case GL_FLOAT:
						if( ArrInt->type() == QMetaType::Float && ArrInt->elementCount() == 1 && ArrInt->stride() == sizeof( float ) )
						{
							mShaderCompilerData.mProgram->setUniformValueArray( UniDat.mLocation, static_cast<const GLfloat *>( ArrDat ), CpyCnt, 1 );
						}
						break;

					case GL_FLOAT_VEC2:
						if( ArrInt->type() == QMetaType::Float && ArrInt->elementCount() == 2 && ArrInt->stride() == 2 * sizeof( float ) )
						{
							mShaderCompilerData.mProgram->setUniformValueArray( UniDat.mLocation, static_cast<const GLfloat *>( ArrDat ), CpyCnt, 2 );
						}
						else if( ArrInt->type() == QMetaType::QVector2D && ArrInt->elementCount() == 1 && ArrInt->stride() == 2 * sizeof( float ) )
						{
							mShaderCompilerData.mProgram->setUniformValueArray( UniDat.mLocation, static_cast<const QVector2D *>( ArrDat ), CpyCnt );
						}
						else if( ArrInt->type() == QMetaType::QPointF && ArrInt->elementCount() == 1 && ArrInt->stride() == 2 * sizeof( float ) )
						{
							glUniform2fv( UniDat.mLocation, CpyCnt, static_cast<const float *>( ArrDat ) );
						}
						break;

					case GL_FLOAT_VEC3:
						if( ArrInt->type() == QMetaType::Float && ArrInt->elementCount() == 3 && ArrInt->stride() == 3 * sizeof( float ) )
						{
							mShaderCompilerData.mProgram->setUniformValueArray( UniDat.mLocation, static_cast<const GLfloat *>( ArrDat ), CpyCnt, 3 );
						}
						else if( ArrInt->type() == QMetaType::QVector3D && ArrInt->elementCount() == 1 && ArrInt->stride() == 3 * sizeof( float ) )
						{
							glUniform3fv( UniDat.mLocation, CpyCnt, static_cast<const float *>( ArrDat ) );
						}
						break;

					case GL_FLOAT_VEC4:
						if( ArrInt->type() == QMetaType::Float && ArrInt->elementCount() == 4 && ArrInt->stride() == 4 * sizeof( float ) )
						{
							mShaderCompilerData.mProgram->setUniformValueArray( UniDat.mLocation, static_cast<const GLfloat *>( ArrDat ), CpyCnt, 4 );
						}
						else if( ArrInt->type() == QMetaType::QVector4D && ArrInt->elementCount() == 1 && ArrInt->stride() == 4 * sizeof( float ) )
						{
							glUniform4fv( UniDat.mLocation, CpyCnt, static_cast<const float *>( ArrDat ) );
						}
						break;

					case GL_INT_VEC2:
						if( ArrInt->type() == QMetaType::Int && ArrInt->elementCount() == 2 && ArrInt->stride() == 2 * sizeof( int ) )
						{
							glUniform2iv( UniDat.mLocation, CpyCnt, static_cast<const int *>( ArrDat ) );
						}
						else if( ArrInt->type() == QMetaType::QPoint && ArrInt->elementCount() == 1 && ArrInt->stride() == 2 * sizeof( int ) )
						{
							glUniform2iv( UniDat.mLocation, CpyCnt, static_cast<const int *>( ArrDat ) );
						}
						break;

					case GL_INT_VEC3:
						if( ArrInt->type() == QMetaType::Int && ArrInt->elementCount() == 3 && ArrInt->stride() == 3 * sizeof( int ) )
						{
							glUniform3iv( UniDat.mLocation, CpyCnt, static_cast<const int *>( ArrDat ) );
						}
						break;

					case GL_INT_VEC4:
						if( ArrInt->type() == QMetaType::Int && ArrInt->elementCount() == 4 && ArrInt->stride() == 4 * sizeof( int ) )
						{
							glUniform4iv( UniDat.mLocation, CpyCnt, static_cast<const int *>( ArrDat ) );
						}
						break;

					case GL_FLOAT_MAT4:
						if( ArrInt->type() == QMetaType::QMatrix4x4 && ArrInt->elementCount() == 1 && ArrInt->stride() == sizeof( QMatrix4x4 ) )
						{
							mShaderCompilerData.mProgram->setUniformValueArray( UniDat.mLocation, static_cast<const QMatrix4x4 *>( ArrDat ), CpyCnt );
						}
						break;
				}
			}

			continue;
		}

		QVariant			UniVar = variant( P );
		QMetaType::Type		UniTyp = QMetaType::Type( UniVar.type() );

		switch( UniDat.mType )
		{
			case GL_FLOAT:
				mShaderCompilerData.mProgram->setUniformValue( UniDat.mLocation, UniVar.toFloat() );
				break;

			case GL_FLOAT_VEC2:
				if( UniTyp == QMetaType::QVector2D )
				{
					mShaderCompilerData.mProgram->setUniformValue( UniDat.mLocation, UniVar.value<QVector2D>() );
				}
				else if( UniTyp == QMetaType::QSize || UniTyp == QMetaType::QSizeF )
				{
					mShaderCompilerData.mProgram->setUniformValue( UniDat.mLocation, UniVar.toSizeF() );
				}
				else if( UniTyp == QMetaType::QPoint || UniTyp == QMetaType::QPointF )
				{
					mShaderCompilerData.mProgram->setUniformValue( UniDat.mLocation, UniVar.toPointF() );
				}
				break;

			case GL_FLOAT_VEC3:
				if( UniTyp == QMetaType::QVector3D )
				{
					mShaderCompilerData.mProgram->setUniformValue( UniDat.mLocation, UniVar.value<QVector3D>() );
				}
				else if( UniTyp == QMetaType::QColor )
				{
					QColor		C = UniVar.value<QColor>();

					mShaderCompilerData.mProgram->setUniformValue( UniDat.mLocation, C.redF(), C.greenF(), C.blueF() );
				}
				break;

			case GL_FLOAT_VEC4:
				if( UniTyp == QMetaType::QVector4D )
				{
					mShaderCompilerData.mProgram->setUniformValue( UniDat.mLocation, UniVar.value<QVector4D>() );
				}
				else if( UniTyp == QMetaType::QColor )
				{
					mShaderCompilerData.mProgram->setUniformValue( UniDat.mLocation, UniVar.value<QColor>() );
				}
				break;

			case GL_INT:
				mShaderCompilerData.mProgram->setUniformValue( UniDat.mLocation, UniVar.toInt() );
				break;

			case GL_INT_VEC2:
			case GL_INT_VEC3:
			case GL_INT_VEC4:
				break;

			case GL_BOOL:
				mShaderCompilerData.mProgram->setUniformValue( UniDat.mLocation, UniVar.toBool() );
				break;

			case GL_BOOL_VEC2:
			case GL_BOOL_VEC3:
			case GL_BOOL_VEC4:
				break;

			case GL_FLOAT_MAT2:
				mShaderCompilerData.mProgram->setUniformValue( UniDat.mLocation, UniVar.value<QMatrix2x2>() );
				break;

			case GL_FLOAT_MAT3:
				mShaderCompilerData.mProgram->setUniformValue( UniDat.mLocation, UniVar.value<QMatrix3x3>() );
				break;

			case GL_FLOAT_MAT4:
				mShaderCompilerData.mProgram->setUniformValue( UniDat.mLocation, UniVar.value<QMatrix4x4>() );
				break;
		}
	}

	mShaderCompilerData.mProgram->release();
}

void EasyShader2DNode::updateOutputPins()
{
	QSize		DstSze;

	for( QSharedPointer<fugio::PinInterface> P : mNode->enumOutputPins() )
	{
		if( P == mPinOutputRender )
		{
			continue;
		}

		fugio::OpenGLTextureInterface	*TexInf = output<fugio::OpenGLTextureInterface *>( P );

		if( !TexInf || !TexInf->dstTexId() )
		{
			continue;
		}

		QSize		TexSze( TexInf->size().x(), TexInf->size().y() );

		if( DstSze.isEmpty() || ( !TexSze.isEmpty() && DstSze != TexSze ) )
		{
			DstSze = TexSze;
		}
	}

	mFBO.setSize( DstSze );

	if( DstSze.isEmpty() )
	{
		return;
	}

	GLuint	FBO = mFBO.fbo();

	if( !FBO )
	{
		return;
	}

	glBindFramebuffer( GL_FRAMEBUFFER, FBO );

	QVector<GLenum>		FrgOut;

	for( QSharedPointer<fugio::PinInterface> P : mNode->enumOutputPins() )
	{
		if( P == mPinOutputRender )
		{
			continue;
		}

		fugio::OpenGLTextureInterface	*TexInf = output<fugio::OpenGLTextureInterface *>( P );

		if( !TexInf || !TexInf->dstTexId() )
		{
			continue;
		}

		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + FrgOut.size(), TexInf->target(), TexInf->dstTexId(), 0 );

		FrgOut << GL_COLOR_ATTACHMENT0 + FrgOut.size();
	}

	Q_ASSERT( glCheckFramebufferStatus( GL_FRAMEBUFFER ) == GL_FRAMEBUFFER_COMPLETE );

#if defined( QOPENGLEXTRAFUNCTIONS_H )
	QOpenGLExtraFunctions	*GLEX = QOpenGLContext::currentContext()->extraFunctions();

	if( GLEX )
	{
		GLEX->glDrawBuffers( FrgOut.size(), FrgOut.constData() );
	}
#endif

	glViewport( 0, 0, mFBO.size().width(), mFBO.size().height() );

	OPENGL_PLUGIN_DEBUG
}

