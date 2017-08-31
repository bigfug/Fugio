#include "easyshader2dnode.h"

#include <QOpenGLContext>

#include <fugio/core/uuid.h>
#include <fugio/text/uuid.h>
#include <fugio/opengl/uuid.h>
#include <fugio/performance.h>

#include "openglplugin.h"

EasyShader2DNode::EasyShader2DNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mQuadGeometry( QOpenGLBuffer::VertexBuffer )
{
	FUGID( PIN_INPUT_VERTEX_SHADER, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_FRAGMENT_SHADER, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_OUTPUT_RENDER, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );
	FUGID( PIN_OUTPUT_TEXTURE, "249f2932-f483-422f-b811-ab679f006381" );
//	FUGID( PIN_XXX_XXX, "ce8d578e-c5a4-422f-b3c4-a1bdf40facdb" );
	//FUGID( PIN_XXX_XXX, "e6bf944e-5f46-4994-bd51-13c2aa6415b7" );
	//FUGID( PIN_XXX_XXX, "a2bbf374-0dc8-42cb-b85a-6a43b58a348f" );
	//FUGID( PIN_XXX_XXX, "51297977-7b4b-4e08-9dea-89a8add4abe0" );
	//FUGID( PIN_XXX_XXX, "c997473a-2016-466b-9128-beacb99870a2" );

	mPinInputTrigger = pinInput( "Trigger", PID_FUGIO_NODE_TRIGGER );

	mValInputShaderVertex = pinInput<fugio::SyntaxErrorInterface *>( "Vertex", mPinInputShaderVertex, PID_SYNTAX_ERROR, PIN_INPUT_VERTEX_SHADER );

	mValInputShaderFragment = pinInput<fugio::SyntaxErrorInterface *>( "Fragment", mPinInputShaderFragment, PID_SYNTAX_ERROR, PIN_INPUT_FRAGMENT_SHADER );

	mPinInputShaderVertex->registerPinInputType( PID_STRING );
	mPinInputShaderFragment->registerPinInputType( PID_STRING );

	mValInputShaderVertex->setHighlighterUuid( SYNTAX_HIGHLIGHTER_GLSL );
	mValInputShaderFragment->setHighlighterUuid( SYNTAX_HIGHLIGHTER_GLSL );

	mValOutputRender = pinOutput<fugio::RenderInterface *>( "Render", mPinOutputRender, PID_RENDER, PIN_OUTPUT_RENDER );

	mPinOutputTexture = pinOutput( "Texture", PIN_OUTPUT_TEXTURE );

	mPinInputShaderVertex->setValue(
		"attribute highp vec2 vertex;\n\n"
		"varying highp vec2 TextureCoords;\n\n"
		"void main( void )\n"
		"{\n"
		"	gl_Position = vec4( vertex, 0, 1 );\n"
		"	TextureCoords = ( vertex * 0.5 ) + 0.5;\n"
		"}\n" );

	mPinInputShaderFragment->setValue(
		"varying highp vec2 TextureCoords;\n\n"
		"void main( void )\n"
		"{\n"
		"	gl_FragColor = vec4( TextureCoords, 0.5, 1 );\n"
		"}\n" );
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

	if( !mVAO.isCreated() )
	{
		mVAO.create();
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
	}

	if( !mShaderCompilerData.mProgram )
	{
		return;
	}

	updateInputPins();

	pinUpdated( mPinOutputRender );

	if( mPinOutputTexture->isConnected() && QOpenGLContext::currentContext() )
	{
		renderToTexture( pTimeStamp );

		pinUpdated( mPinOutputTexture );
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

	static const GLfloat Verticies[] =
	{
		-1, -1,
		-1,  1,
		 1, -1,
		 1,  1
	};

	if( !mVAO.isCreated() )
	{
		glVertexAttribPointer( VertexLocation, 2, GL_FLOAT, GL_FALSE, 0, 0 );

		glEnableVertexAttribArray( VertexLocation );
	}
	else
	{
		mVAO.bind();

		if( !mQuadGeometry.isCreated() )
		{
			if( mQuadGeometry.create() )
			{
				if( mQuadGeometry.bind() )
				{
					mQuadGeometry.allocate( Verticies, sizeof( Verticies ) );
				}
			}

			glVertexAttribPointer( VertexLocation, 2, GL_FLOAT, GL_FALSE, 0, 0 );

			glEnableVertexAttribArray( VertexLocation );

			mQuadGeometry.release();
		}
	}

	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

	if( mVAO.isCreated() )
	{
		mVAO.release();
	}
	else
	{
		glDisableVertexAttribArray( VertexLocation );
	}

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

void EasyShader2DNode::renderToTexture( qint64 pTimeStamp )
{
	fugio::OpenGLTextureInterface	*DstTex = output<fugio::OpenGLTextureInterface *>( mPinOutputTexture );

	if( !DstTex || !DstTex->dstTexId() )
	{
		return;
	}

	glBindFramebuffer( GL_FRAMEBUFFER, DstTex->fbo() );

	GLint		VP[ 4 ];

	glGetIntegerv( GL_VIEWPORT, VP );

	glViewport( 0, 0, DstTex->size().x(), DstTex->size().y() );

	render( pTimeStamp, QUuid() );

	glViewport( VP[ 0 ], VP[ 1 ], VP[ 2 ], VP[ 3 ] );

	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
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
					mShaderCompilerData.mProgram->setUniformValue( UniDat.mLocation, UniVar.value<QColor>() );
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

