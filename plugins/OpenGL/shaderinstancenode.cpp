#include "shaderinstancenode.h"

#include <QMatrix2x2>
#include <QMatrix3x3>
#include <QMatrix4x4>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QOpenGLVertexArrayObject>

#if QT_VERSION >= QT_VERSION_CHECK( 5, 6, 0 )
#include <QOpenGLExtraFunctions>
#endif

#include <QOpenGLFunctions_3_2_Core>

#include <fugio/core/uuid.h>
#include <fugio/opengl/uuid.h>

#include <fugio/performance.h>

#include <fugio/context_interface.h>
#include <fugio/opengl/texture_interface.h>
#include <fugio/colour/colour_interface.h>
#include <fugio/core/variant_interface.h>
#include <fugio/opengl/state_interface.h>
#include <fugio/opengl/buffer_interface.h>
#include <fugio/opengl/buffer_entry_interface.h>
#include <fugio/opengl/vertex_array_object_interface.h>

#include "openglplugin.h"

using namespace fugio;

ShaderInstanceNode::ShaderInstanceNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mFrameBufferId( 0 )
{
	const static QUuid		PII_GEOMETRY( "{6272595e-bb59-407c-9a80-64dc8c99ba22}" );
	const QUuid				PII_STATE = next_uuid();
	const QUuid				PII_SHADER = next_uuid();
	FUGID( PIN_INPUT_VAO,		"439bf26c-821f-4930-aa03-2b62a3511d94" );
	FUGID( PIN_INPUT_PASSES,	"fe19f444-7a5f-47e6-ae83-ceb43b8ae915" );
	FUGID( PIN_INPUT_PASSVAR,	"a9b8d8d8-e4aa-4a99-b4eb-ea22f1945c63" );

	pinInput( "Trigger", PID_FUGIO_NODE_TRIGGER );

	mPinShader = pinInput( "Shader", PII_SHADER );

	mPinState = pinInput( "State", PII_STATE );

	mPinInputVAO = pinInput( "VAO", PIN_INPUT_VAO );

	mPinInputPasses = pinInput( "Passes", PIN_INPUT_PASSES );

	mPinInputPasses->setValue( 1 );

	mPinInputPassVar = pinInput( "Pass Uniform", PIN_INPUT_PASSVAR );

	mPinInputPassVar->setValue( "Pass" );

	mPinShader->registerPinInputType( PID_OPENGL_SHADER );
	mPinInputVAO->registerPinInputType( PID_OPENGL_VERTEX_ARRAY_OBJECT );

	QSharedPointer<fugio::PinInterface>	PinGeometry = pinInput( "Render", PII_GEOMETRY );

	mPinShader->registerPinInputType( PID_RENDER );

	mOutputGeometry = pinOutput<fugio::RenderInterface *>( "Render", mPinOutputGeometry, PID_RENDER );

	pinOutput( "Output1", next_uuid() );

	mPinState->setDescription( tr( "The OpenGL rendering state to apply" ) );

	PinGeometry->setDescription( tr( "The input 3D Geometry to render with this shader" ) );

	mPinOutputGeometry->setDescription( tr( "The output geometry that can be connected to an OpenGL Window"));
}

bool ShaderInstanceNode::initialise()
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

void ShaderInstanceNode::inputsUpdated( qint64 pTimeStamp )
{
	OPENGL_DEBUG( mNode->name() );

	OpenGLShaderInterface	*Shader = input<OpenGLShaderInterface *>( mPinShader );

	if( !Shader || !Shader->isLinked() )
	{
		mNode->setStatus( fugio::NodeInterface::Error );
		mNode->setStatusMessage( tr( "No shader or not linked" ) );

		return;
	}

	if( mPinShader->isUpdated( pTimeStamp ) )
	{
		mNode->setStatus( fugio::NodeInterface::Initialised );
		mNode->setStatusMessage( QString() );
	}

	if( mNode->status() == fugio::NodeInterface::Error )
	{
		return;
	}

	//-------------------------------------------------------------------------

	if( mPinOutputGeometry->isConnected() && mPinOutputGeometry->isConnectedToActiveNode() )
	{
		pinUpdated( mPinOutputGeometry );
	}

	//-------------------------------------------------------------------------

	const int	Passes = variant( mPinInputPasses ).toInt();

	if( Passes <= 0 )
	{
		return;
	}

	//-------------------------------------------------------------------------

	int		W, H, D;

	QList< QSharedPointer<fugio::PinInterface> >	InpPinLst = mNode->enumInputPins();
	QList< QSharedPointer<fugio::PinInterface> >	OutPinLst = mNode->enumOutputPins();

	if( !activeBufferCount( OutPinLst ) )
	{
		return;
	}

	if( !OpenGLPlugin::hasContextStatic() )
	{
		return;
	}

	initializeOpenGLFunctions();

	//-------------------------------------------------------------------------

	fugio::Performance	Perf( mNode, "inputsUpdated", pTimeStamp );

	//-------------------------------------------------------------------------

	if( Passes > 1 )
	{
		for( QSharedPointer<fugio::PinInterface> &OutPin : OutPinLst )
		{
			OpenGLTextureInterface		*OutTex = output<OpenGLTextureInterface *>( OutPin );

			if( !OutTex )
			{
				continue;
			}

			if( OutTex->isDoubleBuffered() )
			{
				if( !OutTex->dstTexId() )
				{
					OutTex->update();
				}

				if( OutTex->dstTexId() && !OutTex->srcTexId() )
				{
					OutTex->swapTexture();

					OutTex->update();
				}
			}
		}
	}

	//-------------------------------------------------------------------------

	glUseProgram( Shader->programId() );

	OPENGL_DEBUG( mNode->name() );

	//-------------------------------------------------------------------------
	// Bind our Vertex Array Object (if supported)

	fugio::VertexArrayObjectInterface	*VAO = input<fugio::VertexArrayObjectInterface *>( mPinInputVAO );

	if( VAO )
	{
		VAO->vaoBind();
	}

	//-------------------------------------------------------------------------

	QList<ShaderBindData>		Bindings;

	bindUniforms( Bindings );

	//-------------------------------------------------------------------------

	OpenGLStateInterface		*State = input<OpenGLStateInterface *>( mPinState );

	if( State )
	{
		State->stateBegin();
	}

	//-------------------------------------------------------------------------

#if defined( QOPENGLEXTRAFUNCTIONS_H )
	QOpenGLExtraFunctions	*GLEX = QOpenGLContext::currentContext()->extraFunctions();
#endif

	GLint PassVarLocation = glGetUniformLocation( Shader->programId(), variant( mPinInputPassVar ).toString().toLatin1().constData() );

	for( int Pass = 0 ; Pass < Passes ; Pass++ )
	{
		if( PassVarLocation != -1 )
		{
			glUniform1i( PassVarLocation, Pass );
		}

		//-------------------------------------------------------------------------
		// Bind all output buffers

		QVector<GLenum>		Buffers;

		bindOutputBuffers( Buffers, OutPinLst, W, H, D );

		if( mFrameBufferId )
		{
	#if !defined( GL_ES_VERSION_2_0 )
			if( Buffers.empty() )
			{
				glDeleteFramebuffers( 1, &mFrameBufferId );

				mFrameBufferId = 0;
			}
			else
			{
				//bool		HasDepth = Buffers.contains( GL_DEPTH_ATTACHMENT );

				Buffers.removeAll( GL_DEPTH_ATTACHMENT );

				glBindFramebuffer( GL_FRAMEBUFFER, mFrameBufferId );

				if( Buffers.isEmpty() )
				{
					Buffers.append( GL_NONE );
				}

#if defined( QOPENGLEXTRAFUNCTIONS_H )
				if( GLEX )
				{
					GLEX->glDrawBuffers( Buffers.size(), Buffers.data() );
				}
#endif

				OPENGL_DEBUG( mNode->name() );

				GLenum	GLerr;

				if( ( GLerr = glCheckFramebufferStatus( GL_FRAMEBUFFER ) ) != GL_FRAMEBUFFER_COMPLETE )
				{
					qDebug() << "glCheckFramebufferStatus( GL_FRAMEBUFFER ) =" << OpenGLPlugin::framebufferError( GLerr );

					glBindFramebuffer( GL_FRAMEBUFFER, 0 );

					if( VAO )
					{
						VAO->vaoRelease();
					}

					mNode->setStatus( fugio::NodeInterface::Error );
					mNode->setStatusMessage( QString( "glCheckFramebufferStatus( GL_FRAMEBUFFER ) = %1" ).arg( OpenGLPlugin::framebufferError( GLerr ) ) );

					break;
				}
			}
	#endif
		}

		//-------------------------------------------------------------------------

		if( true )
		{
			GLint		VP[ 4 ];

			if( mFrameBufferId )
			{
				glGetIntegerv( GL_VIEWPORT, VP );

				glViewport( 0, 0, W, H );
			}

			for( QSharedPointer<fugio::PinInterface> &InpPin : InpPinLst )
			{
				fugio::RenderInterface		*Geometry = input<fugio::RenderInterface *>( InpPin );

				if( Geometry )
				{
					Geometry->render( pTimeStamp );
				}
			}

			OPENGL_DEBUG( mNode->name() );

			if( mFrameBufferId )
			{
				glViewport( VP[ 0 ], VP[ 1 ], VP[ 2 ], VP[ 3 ] );
			}
		}

		//-------------------------------------------------------------------------

		if( mFrameBufferId )
		{
			glBindFramebuffer( GL_FRAMEBUFFER, 0 );
		}

		//-------------------------------------------------------------------------

		releaseInputTextures( Bindings );

		//-------------------------------------------------------------------------
		// Swap textures

		if( Pass + 1 < Passes )
		{
			for( QSharedPointer<fugio::PinInterface> &OutPin : OutPinLst )
			{
				OpenGLTextureInterface		*OutTex = output<OpenGLTextureInterface *>( OutPin );

				if( !OutTex )
				{
					continue;
				}

				OutTex->swapTexture();
			}

			for( ShaderUniformMap::const_iterator it = Shader->uniformMap().begin() ; it != Shader->uniformMap().end() ; it++ )
			{
				const ShaderUniformData				&UniformData = it.value();

				if( UniformData.mSampler && UniformData.mSize == 1 )
				{
					for( QList<ShaderBindData>::iterator it = Bindings.begin() ; it != Bindings.end() ; it++ )
					{
						ShaderBindData	&BindData = *it;

						if( BindData.mUnit == GLenum( GL_TEXTURE0 + UniformData.mTextureBinding ) )
						{
							glActiveTexture( BindData.mUnit );

							BindData.mTexture->srcBind();

							//glEnable( BindData.mTarget );

							glUniform1i( UniformData.mLocation, UniformData.mTextureBinding );
						}
					}
				}
			}
		}
	}

	//-------------------------------------------------------------------------

	if( State )
	{
		State->stateEnd();
	}

	//-------------------------------------------------------------------------

	glUseProgram( 0 );

	//-------------------------------------------------------------------------

	if( VAO )
	{
		VAO->vaoRelease();
	}

	OPENGL_DEBUG( mNode->name() );

	//-------------------------------------------------------------------------

	for( QSharedPointer<fugio::PinInterface> &P : OutPinLst )
	{
		if( !P->isConnectedToActiveNode() )
		{
			continue;
		}

		if( output<OpenGLTextureInterface *>( P ) )
		{
			pinUpdated( P );
		}
	}
}

QStringList ShaderInstanceNode::availableInputPins() const
{
	QStringList		PinLst;

	PinLst << NodeControlBase::availableInputPins();

	if( mPinShader->isConnected() && mPinShader->connectedPin()->hasControl() )
	{
		OpenGLShaderInterface	*S = qobject_cast<OpenGLShaderInterface *>( mPinShader->connectedPin()->control()->qobject() );

		if( S )
		{
			PinLst << S->uniformMap().keys();
			//PinLst << S->attributeMap().keys();
		}
	}

	for( QSharedPointer<fugio::PinInterface> P : mNode->enumInputPins() )
	{
		PinLst.removeAll( P->name() );
	}

	qSort( PinLst );

	return( PinLst );
}

QList<QUuid> ShaderInstanceNode::pinAddTypesInput() const
{
	static QList<QUuid>		PinLst =
	{
		PID_RENDER,
		PID_OPENGL_STATE,
		PID_OPENGL_BUFFER,
		PID_OPENGL_TEXTURE,
		PID_BOOL,
		PID_COLOUR,
		PID_FLOAT,
		PID_INTEGER,
		PID_TRIGGER,
		PID_POINT,
		PID_OPENGL_BUFFER_ENTRY_PROXY
	};

	return( PinLst );
}

bool ShaderInstanceNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	if( pPin->direction() == PIN_INPUT )
	{
		return( pinAddTypesOutput().contains( pPin->controlUuid() ) );
	}

	return( pinAddTypesInput().contains( pPin->controlUuid() ) );
}

void ShaderInstanceNode::bindTexture( QList<fugio::ShaderBindData> &Bindings, QSharedPointer<PinControlInterface> PinControl, const fugio::ShaderUniformData &UniformData )
{
	OpenGLTextureInterface		*PinTexture = qobject_cast<OpenGLTextureInterface *>( PinControl->qobject() );

	if( !PinTexture )
	{
		return;
	}

	ShaderBindData	BindData;

	BindData.mUnit    = GL_TEXTURE0 + UniformData.mTextureBinding;
	BindData.mTexture = PinTexture;

	if( BindData.mTexture && BindData.mTexture->srcTexId() )
	{
		Bindings.append( BindData );

		glActiveTexture( BindData.mUnit );

		PinTexture->srcBind();

		//glEnable( BindData.mTarget );

		glUniform1i( UniformData.mLocation, UniformData.mTextureBinding );

		OPENGL_DEBUG( mNode->name() );

		//qDebug() << PIN->name() << "Bind GL_TEXTURE" << UniformData.mTextureBinding << UniformData.mLocation << BindData.mTarget << PinTexture->textureId();
	}
}

int ShaderInstanceNode::activeBufferCount( QList< QSharedPointer<fugio::PinInterface> > &OutPinLst ) const
{
	int		ActiveBufferCount = 0;

	for( QSharedPointer<fugio::PinInterface> &OutPin : OutPinLst )
	{
		OpenGLTextureInterface		*OutTex = output<OpenGLTextureInterface *>( OutPin );

		if( OutTex && !OutTex->size().isNull() )
		{
			ActiveBufferCount++;

			continue;
		}

		fugio::OpenGLBufferInterface	*OutBuf = output<fugio::OpenGLBufferInterface *>( OutPin );

		if( OutBuf && OutBuf->buffer() && OutBuf->buffer()->isCreated() )
		{
			ActiveBufferCount++;

			continue;
		}
	}

	return( ActiveBufferCount );
}

void ShaderInstanceNode::bindOutputBuffers( QVector<GLenum> &Buffers, QList< QSharedPointer<fugio::PinInterface> > &OutPinLst, int &W, int &H, int &D )
{
#if !defined( QT_OPENGL_ES_2 )
	QOpenGLFunctions_3_2_Core	*GL32 = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_2_Core>();

	if( GL32 && !GL32->initializeOpenGLFunctions() )
	{
		GL32 = Q_NULLPTR;
	}
#endif

	for( QSharedPointer<fugio::PinInterface> &OutPin : OutPinLst )
	{
		OpenGLTextureInterface		*OutTex = output<OpenGLTextureInterface *>( OutPin );

		if( !OutTex )
		{
			continue;
		}

		if( !OutTex->dstTexId() )
		{
			OutTex->update();

			if( !OutTex->dstTexId() )
			{
				continue;
			}
		}

		if( Buffers.empty() )
		{
			W = OutTex->size().x();
			H = OutTex->size().y();
			D = OutTex->size().z();
		}

		//glBindTexture( DstTexture->target(), 0 );

		if( !mFrameBufferId )
		{
			glGenFramebuffers( 1, &mFrameBufferId );

			if( !mFrameBufferId )
			{
				continue;
			}
		}

		glBindFramebuffer( GL_FRAMEBUFFER, mFrameBufferId );

		switch( OutTex->target() )
		{
			case QOpenGLTexture::Target1D:
#if !defined( QT_OPENGL_ES_2 )
				if( GL32 )
				{
					GL32->glFramebufferTexture1D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + Buffers.size(), OutTex->target(), OutTex->dstTexId(), 0 );
				}
#endif

				Buffers.append( GL_COLOR_ATTACHMENT0 + Buffers.size() );
				break;

			case GL_TEXTURE_2D:
			case QOpenGLTexture::TargetRectangle:
				if( !OutTex->isDepthTexture() )
				{
					glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + Buffers.size(), OutTex->target(), OutTex->dstTexId(), 0 );

					Buffers.append( GL_COLOR_ATTACHMENT0 + Buffers.size() );
				}
				else
				{
					glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, OutTex->target(), OutTex->dstTexId(), 0 );

					Buffers.append( GL_DEPTH_ATTACHMENT );
				}
				break;

			case GL_TEXTURE_CUBE_MAP:
				if( !OutTex->isDepthTexture() )
				{
					glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + Buffers.size(), OutTex->target(), OutTex->dstTexId(), 0 );

					Buffers.append( GL_COLOR_ATTACHMENT0 + Buffers.size() );
				}
				else
				{
					glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, OutTex->target(), OutTex->dstTexId(), 0 );

					Buffers.append( GL_DEPTH_ATTACHMENT );
				}
				break;

			case GL_TEXTURE_3D:
#if !defined( QT_OPENGL_ES_2 )
				if( GL32 )
				{
					GL32->glFramebufferTexture3D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + Buffers.size(), OutTex->target(), OutTex->dstTexId(), 0, 0 );
				}
#endif

				Buffers.append( GL_COLOR_ATTACHMENT0 + Buffers.size() );
				break;

			default:
				break;
		}
	}

	OPENGL_DEBUG( mNode->name() );
}

void ShaderInstanceNode::render( qint64 pTimeStamp, QUuid pSourcePinId )
{
	Q_UNUSED( pSourcePinId )

	fugio::Performance	Perf( mNode, "drawGeometry", pTimeStamp );

	if( !mNode->isInitialised() )
	{
		return;
	}

	initializeOpenGLFunctions();

	OpenGLShaderInterface	*Shader = input<OpenGLShaderInterface *>( mPinShader );

	if( !Shader || !Shader->isLinked() )
	{
		return;
	}

	if( mNode->status() != fugio::NodeInterface::Initialised )
	{
		mNode->setStatus( fugio::NodeInterface::Initialised );
		mNode->setStatusMessage( QString() );
	}

	QList< QSharedPointer<fugio::PinInterface> >	InpPinLst = mNode->enumInputPins();

	//-------------------------------------------------------------------------
	// Bind our Vertex Array Object (if supported)

	fugio::VertexArrayObjectInterface	*VAO = input<fugio::VertexArrayObjectInterface *>( mPinInputVAO );

	if( VAO )
	{
		VAO->vaoBind();
	}

	//-------------------------------------------------------------------------

	glUseProgram( Shader->programId() );

	OPENGL_DEBUG( mNode->name() );

	QList<ShaderBindData>		Bindings;

	bindUniforms( Bindings );

	OPENGL_DEBUG( mNode->name() );

	//bindAttributes();

	OPENGL_DEBUG( mNode->name() );

	//-------------------------------------------------------------------------

	OpenGLStateInterface		*CurrentState = 0;
	OpenGLStateInterface		*NextState;
	fugio::RenderInterface		*Geometry;

	//-------------------------------------------------------------------------

	for( QList< QSharedPointer<fugio::PinInterface> >::iterator it = InpPinLst.begin() ; it != InpPinLst.end() ; it++ )
	{
		QSharedPointer<fugio::PinInterface>	InpPin = *it;

		if( !InpPin->isConnectedToActiveNode() )
		{
			continue;
		}

		QSharedPointer<PinControlInterface>		 PinCtl = InpPin->connectedPin()->control();

		if( PinCtl.isNull() )
		{
			continue;
		}

		if( ( Geometry = qobject_cast<fugio::RenderInterface *>( PinCtl->qobject() ) ) != 0 )
		{
			Geometry->render( pTimeStamp );

			OPENGL_DEBUG( mNode->name() );

			continue;
		}

		if( ( NextState = qobject_cast<OpenGLStateInterface *>( PinCtl->qobject() ) ) != 0 )
		{
			if( CurrentState != 0 )
			{
				CurrentState->stateEnd();
			}

			CurrentState = NextState;

			CurrentState->stateBegin();

			OPENGL_DEBUG( mNode->name() );
		}
	}

	if( CurrentState != 0 )
	{
		CurrentState->stateEnd();
	}

	OPENGL_DEBUG( mNode->name() );

	//-------------------------------------------------------------------------

	glUseProgram( 0 );

	//-------------------------------------------------------------------------

	releaseInputTextures( Bindings );

	if( VAO )
	{
		VAO->vaoRelease();
	}

	OPENGL_DEBUG( mNode->name() );
}

void ShaderInstanceNode::releaseInputTextures( QList<ShaderBindData> &Bindings )
{
	for( QList<ShaderBindData>::iterator it = Bindings.begin() ; it != Bindings.end() ; it++ )
	{
		ShaderBindData	&BindData = *it;

		glActiveTexture( BindData.mUnit );

		BindData.mTexture->release();
	}

	glActiveTexture( GL_TEXTURE0 );
}

void ShaderInstanceNode::bindUniforms( QList<ShaderBindData> &Bindings )
{
	bool		NumberOK;

#if defined( QOPENGLEXTRAFUNCTIONS_H )
	QOpenGLExtraFunctions	*GLEX = QOpenGLContext::currentContext()->extraFunctions();
#endif

	OpenGLShaderInterface					*Shader = input<OpenGLShaderInterface *>( mPinShader );
	fugio::NodeInterface					*CompilerNode = mPinShader->connectedNode();

	for( ShaderUniformMap::const_iterator it = Shader->uniformMap().begin() ; it != Shader->uniformMap().end() ; it++ )
	{
		const ShaderUniformData				&UniformData = it.value();
		QSharedPointer<fugio::PinInterface>	 PIN = mNode->findPinByName( it.key() );
		QSharedPointer<PinControlInterface>	 PinControl;

		if( PIN && PIN->isConnected() )
		{
			PinControl = PIN->connectedPin()->control();
		}

		if( !PinControl )
		{
			QSharedPointer<fugio::PinInterface>	 PIN2 = CompilerNode->findInputPinByName( it.key() );

			if( PIN2 && PIN2->isConnected() )
			{
				PIN = PIN2;

				PinControl = PIN->connectedPin()->control();
			}
		}

		QVariant		PinVar;

		if( !PinControl && PIN )
		{
			PinVar = PIN->value();
		}

		if( PinControl && UniformData.mSampler && UniformData.mSize == 1 )
		{
			bindTexture( Bindings, PinControl, UniformData );

			continue;
		}

//		if( mLastShaderLoad < pTimeStamp && !PIN->isUpdated( pTimeStamp ) )
//		{
//			if( PIN->controlUuid() == PID_TRIGGER )
//			{
//				switch( UniformData.mType )
//				{
//					case GL_BOOL:
//						glUniform1f( UniformData.mLocation, false );
//						break;
//				}
//			}

//			continue;
//		}

		if( UniformData.mSize > 1 )
		{
			if( !PinControl )
			{
				continue;
			}

			fugio::VariantInterface	*ArrInt = qobject_cast<fugio::VariantInterface *>( PinControl->qobject() );

			if( ArrInt )
			{
				int			CpyCnt = qMin( ArrInt->variantCount(), UniformData.mSize );

				if( !CpyCnt )
				{
					continue;
				}

				void		*ArrDat = ArrInt->variantArray();

				if( !ArrDat )
				{
					continue;
				}

				switch( UniformData.mType )
				{
					case GL_FLOAT:
						if( ArrInt->variantType() == QMetaType::Float && ArrInt->variantElementCount() == 1 && ArrInt->variantStride() == sizeof( float ) )
						{
							glUniform1fv( UniformData.mLocation, CpyCnt, static_cast<float *>( ArrDat ) );
						}
						break;

					case GL_FLOAT_VEC2:
						if( ArrInt->variantType() == QMetaType::Float && ArrInt->variantElementCount() == 2 && ArrInt->variantStride() == 2 * sizeof( float ) )
						{
							glUniform2fv( UniformData.mLocation, CpyCnt, static_cast<float *>( ArrDat ) );
						}
						else if( ArrInt->variantType() == QMetaType::QVector2D && ArrInt->variantElementCount() == 1 && ArrInt->variantStride() == 2 * sizeof( float ) )
						{
							glUniform2fv( UniformData.mLocation, CpyCnt, static_cast<float *>( ArrDat ) );
						}
						else if( ArrInt->variantType() == QMetaType::QPointF && ArrInt->variantElementCount() == 1 && ArrInt->variantStride() == 2 * sizeof( float ) )
						{
							glUniform2fv( UniformData.mLocation, CpyCnt, static_cast<float *>( ArrDat ) );
						}
						break;

					case GL_FLOAT_VEC3:
						if( ArrInt->variantType() == QMetaType::Float && ArrInt->variantElementCount() == 3 && ArrInt->variantStride() == 3 * sizeof( float ) )
						{
							glUniform3fv( UniformData.mLocation, CpyCnt, static_cast<float *>( ArrDat ) );
						}
						else if( ArrInt->variantType() == QMetaType::QVector3D && ArrInt->variantElementCount() == 1 && ArrInt->variantStride() == 3 * sizeof( float ) )
						{
							glUniform3fv( UniformData.mLocation, CpyCnt, static_cast<float *>( ArrDat ) );
						}
						break;

					case GL_FLOAT_VEC4:
						if( ArrInt->variantType() == QMetaType::Float && ArrInt->variantElementCount() == 4 && ArrInt->variantStride() == 4 * sizeof( float ) )
						{
							glUniform4fv( UniformData.mLocation, CpyCnt, static_cast<float *>( ArrDat ) );
						}
						else if( ArrInt->variantType() == QMetaType::QVector4D && ArrInt->variantElementCount() == 1 && ArrInt->variantStride() == 4 * sizeof( float ) )
						{
							glUniform4fv( UniformData.mLocation, CpyCnt, static_cast<float *>( ArrDat ) );
						}
						break;

					case GL_INT_VEC2:
						if( ArrInt->variantType() == QMetaType::Int && ArrInt->variantElementCount() == 2 && ArrInt->variantStride() == 2 * sizeof( int ) )
						{
							glUniform2iv( UniformData.mLocation, CpyCnt, static_cast<int *>( ArrDat ) );
						}
						else if( ArrInt->variantType() == QMetaType::QPoint && ArrInt->variantElementCount() == 1 && ArrInt->variantStride() == 2 * sizeof( int ) )
						{
							glUniform2iv( UniformData.mLocation, CpyCnt, static_cast<int *>( ArrDat ) );
						}
						break;

					case GL_INT_VEC3:
						if( ArrInt->variantType() == QMetaType::Int && ArrInt->variantElementCount() == 3 && ArrInt->variantStride() == 3 * sizeof( int ) )
						{
							glUniform3iv( UniformData.mLocation, CpyCnt, static_cast<int *>( ArrDat ) );
						}
						break;

					case GL_INT_VEC4:
						if( ArrInt->variantType() == QMetaType::Int && ArrInt->variantElementCount() == 4 && ArrInt->variantStride() == 4 * sizeof( int ) )
						{
							glUniform4iv( UniformData.mLocation, CpyCnt, static_cast<int *>( ArrDat ) );
						}
						break;

					case GL_FLOAT_MAT4:
						if( ArrInt->variantType() == QMetaType::QMatrix4x4 && ArrInt->variantElementCount() == 1 && ArrInt->variantStride() == 16 * sizeof( float ) )
						{
							glUniformMatrix4fv( UniformData.mLocation, CpyCnt, GL_FALSE, static_cast<float *>( ArrDat ) );
						}
						break;
				}
			}

			continue;
		}

		// It's important to check for InterfaceColour first, as it also implements VariantInterface

		if( PinControl )
		{
			ColourInterface		*PinColour = qobject_cast<ColourInterface *>( PinControl->qobject() );

			if( PinColour )
			{
				switch( UniformData.mType )
				{
					case GL_FLOAT_VEC3:
						glUniform3f( UniformData.mLocation, PinColour->colour().redF(), PinColour->colour().greenF(), PinColour->colour().blueF() );
						break;

					case GL_FLOAT_VEC4:
						glUniform4f( UniformData.mLocation, PinColour->colour().redF(), PinColour->colour().greenF(), PinColour->colour().blueF(), PinColour->colour().alphaF() );
						break;
				}

				OPENGL_DEBUG( mNode->name() );

				continue;
			}
		}

		if( PinControl )
		{
			VariantInterface		*PinVariant = qobject_cast<VariantInterface *>( PinControl->qobject() );

			if( PinVariant )
			{
				PinVar = PinVariant->variant();
			}
		}

		switch( UniformData.mType )
		{
			case GL_BOOL:
				{
					GLboolean		NewVal = PinVar.toBool();

#if defined( QOPENGLEXTRAFUNCTIONS_H )
					if( GLEX )
					{
						GLEX->glUniform1ui( UniformData.mLocation, NewVal );
					}
					else
#endif
					{
						glUniform1i( UniformData.mLocation, NewVal );
					}
				}
				break;

			case GL_INT:
				{
					GLint		NewVal = PinVar.toInt( &NumberOK );

					if( NumberOK )
					{
						glUniform1i( UniformData.mLocation, NewVal );
					}
				}
				break;

			case GL_INT_VEC2:
				{
					if( PinVar.type() == QVariant::Point )
					{
						QPoint		PinDat = PinVar.toPoint();

						glUniform2i( UniformData.mLocation, PinDat.x(), PinDat.y() );
					}
					else if( PinVar.type() == QVariant::PointF )
					{
						QPointF		PinDat = PinVar.toPointF();

						glUniform2i( UniformData.mLocation, PinDat.x(), PinDat.y() );
					}
					else if( PinVar.type() == QVariant::Size )
					{
						QSize		PinDat = PinVar.toSize();

						glUniform2i( UniformData.mLocation, PinDat.width(), PinDat.height() );
					}
					else if( PinVar.type() == QVariant::SizeF )
					{
						QSizeF		PinDat = PinVar.toSizeF();

						glUniform2i( UniformData.mLocation, PinDat.width(), PinDat.height() );
					}
					else // if( PinVar.type() == QVariant::Vector2D )
					{
						QVector2D		Vec2 = PinVar.value<QVector2D>();

						glUniform2i( UniformData.mLocation, Vec2.x(), Vec2.y() );
					}
				}
				break;

			case GL_FLOAT:
				{
					GLfloat		NewVal = PinVar.toFloat( &NumberOK );

					if( NumberOK )
					{
						glUniform1f( UniformData.mLocation, NewVal );
					}
				}
				break;

			case GL_FLOAT_VEC2:
				{
					if( PinVar.type() == QVariant::Point )
					{
						QPoint		PinDat = PinVar.toPoint();

						glUniform2f( UniformData.mLocation, PinDat.x(), PinDat.y() );
					}
					else if( PinVar.type() == QVariant::PointF )
					{
						QPointF		PinDat = PinVar.toPointF();

						glUniform2f( UniformData.mLocation, PinDat.x(), PinDat.y() );
					}
					else if( PinVar.type() == QVariant::Size )
					{
						QSize		PinDat = PinVar.toSize();

						glUniform2f( UniformData.mLocation, PinDat.width(), PinDat.height() );
					}
					else if( PinVar.type() == QVariant::SizeF )
					{
						QSizeF		PinDat = PinVar.toSizeF();

						glUniform2f( UniformData.mLocation, PinDat.width(), PinDat.height() );
					}
					else // if( PinVar.type() == QVariant::Vector2D )
					{
						QVector2D		Vec2 = PinVar.value<QVector2D>();

						glUniform2f( UniformData.mLocation, Vec2.x(), Vec2.y() );
					}
				}
				break;

			case GL_FLOAT_VEC3:
				{
					QVector3D		Vec3 = PinVar.value<QVector3D>();

					glUniform3f( UniformData.mLocation, Vec3.x(), Vec3.y(), Vec3.z() );
				}
				break;

			case GL_FLOAT_VEC4:
				{
					QVector4D		Vec4 = PinVar.value<QVector4D>();

					glUniform4f( UniformData.mLocation, Vec4.x(), Vec4.y(), Vec4.z(), Vec4.w() );
				}
				break;

			case GL_FLOAT_MAT2:
				{
					QMatrix4x4		Mat4 = PinVar.value<QMatrix4x4>();
					QMatrix2x2		Mat2 = Mat4.toGenericMatrix<2,2>();

					glUniformMatrix2fv( UniformData.mLocation, 1, GL_FALSE, Mat2.constData() );
				}
				break;

			case GL_FLOAT_MAT3:
				{
					QMatrix4x4		Mat4 = PinVar.value<QMatrix4x4>();
					QMatrix3x3		Mat3 = Mat4.toGenericMatrix<3,3>();

					//qDebug() << Mat4;
					//qDebug() << Mat3;

					glUniformMatrix3fv( UniformData.mLocation, 1, GL_FALSE, Mat3.constData() );
				}
				break;

			case GL_FLOAT_MAT4:
				{
					QMatrix4x4		NewVal = PinVar.value<QMatrix4x4>();

					glUniformMatrix4fv( UniformData.mLocation, 1, GL_FALSE, NewVal.constData() );
				}
				break;
		}

		OPENGL_DEBUG( mNode->name() );

		if( PIN )
		{
			if( PIN->controlUuid() == PID_TRIGGER )
			{
				switch( UniformData.mType )
				{
					case GL_BOOL:
						glUniform1f( UniformData.mLocation, true );
						break;
				}

				OPENGL_DEBUG( mNode->name() );

				continue;
			}
		}
	}

	OPENGL_DEBUG( mNode->name() );
}

QList<NodeControlInterface::AvailablePinEntry> ShaderInstanceNode::availableOutputPins() const
{
	QList<NodeControlInterface::AvailablePinEntry>		PinLst;

	return( PinLst );
}

QList<QUuid> ShaderInstanceNode::pinAddTypesOutput() const
{
	static QList<QUuid>		PinLst;

	if( PinLst.isEmpty() )
	{
		PinLst << QUuid();
	}

	return( PinLst );
}
