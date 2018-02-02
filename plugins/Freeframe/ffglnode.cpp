#include "ffglnode.h"

#include <QCryptographicHash>

#include <QOpenGLContext>

#include "freeframeplugin.h"

#include <fugio/context_interface.h>

#include <fugio/opengl/uuid.h>

FFGLNode::FFGLNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mInstanceId( 0 )
{
	FUGID( PIN_INPUT_RENDER, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_OUTPUT_RENDER, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );
	FUGID( PIN_OUTPUT_TEXTURE, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );

	QUuid		InputUuid( "{6618BE23-94BD-4706-817A-6AB4736924D9}" );
	QUuid		ParamUuid( "{973ADEB3-F633-4AFA-B67E-1153DB0069BB}" );

	pinInput( "Trigger", PID_FUGIO_NODE_TRIGGER );

	mPinInputRender = pinInput( "Render", PIN_INPUT_RENDER );

	mValOutputRender = pinOutput<fugio::NodeRenderInterface *>( "Render", mPinOutputRender, PID_RENDER, PIN_OUTPUT_RENDER );

	mPinOutputTexture = pinOutput( "Texture", PIN_OUTPUT_TEXTURE );

	if( ( mLibrary = FreeframePlugin::findPluginInfo( mNode->controlUuid() ) ) )
	{
		if( mLibrary->initialise() )
		{
			if( mLibrary->maxInputFrames() > 1 )
			{
				for( int i = 0 ; i < mLibrary->maxInputFrames() ; i++ )
				{
					mInputs.append( pinInput( QString( "Texture %1" ).arg( i ), InputUuid ) );

					InputUuid = QUuid::fromRfc4122( QCryptographicHash::hash( InputUuid.toRfc4122(), QCryptographicHash::Md5 ) );
				}
			}
			else if( mLibrary->maxInputFrames() > 0 )
			{
				mInputs.append( pinInput( "Texture", InputUuid ) );
			}

			//-------------------------------------------------------------------------

			for( const FreeframeLibrary::ParamEntry &PrmEnt : mLibrary->params() )
			{
				QSharedPointer<fugio::PinInterface>		PrmPin = pinInput( PrmEnt.mName, ParamUuid );

				PrmPin->setValue( PrmEnt.mDefault );

				mParams << PrmPin;

				ParamUuid = QUuid::fromRfc4122( QCryptographicHash::hash( ParamUuid.toRfc4122(), QCryptographicHash::Md5 ) );
			}
		}
	}
}

bool FFGLNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	if( !QOpenGLContext::currentContext() )
	{
		return( false );
	}

	initializeOpenGLFunctions();

	return( true );
}

bool FFGLNode::deinitialise()
{
	return( NodeControlBase::deinitialise() );
}

void FFGLNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	if( !QOpenGLContext::currentContext() )
	{
		return;
	}

	initializeOpenGLFunctions();

	FF_Main_FuncPtr		MainFunc = mLibrary->func();

	if( !MainFunc )
	{
		return;
	}

	fugio::OpenGLTextureInterface	*DstTex = output<fugio::OpenGLTextureInterface *>( mPinOutputTexture );

	if( !DstTex || !DstTex->dstTexId() )
	{
		return;
	}

	const QSize			DstSze = QSize( DstTex->size().x(), DstTex->size().y() );

	FFMixed		PMU;

	if( mSize != DstSze )
	{
		if( mInstanceId )
		{
			PMU.UIntValue = 0;

			PMU = MainFunc( FF_DEINSTANTIATEGL, PMU, mInstanceId );

			mInstanceId = 0;
		}

		FFGLViewportStruct	VPS;

		VPS.x = 0;
		VPS.y = 0;
		VPS.width  = DstSze.width();
		VPS.height = DstSze.height();

		PMU.PointerValue = &VPS;

		PMU = MainFunc( FF_INSTANTIATEGL, PMU, 0 );

		if( PMU.UIntValue == FF_FAIL )
		{
			return;
		}

		mSize = DstSze;

		if( PMU.UIntValue != FF_FAIL )
		{
			mInstanceId = PMU.PointerValue;
		}
	}

	if( !mInstanceId )
	{
		return;
	}

	SetParameterStructTag	PrmSet;

	for( int i = 0 ; i < mParams.size() ; i++ )
	{
		QSharedPointer<fugio::PinInterface>		PrmPin = mParams.at( i );
		FreeframeLibrary::ParamEntry			PrmEnt = mLibrary->params().at( i );

		QVariant		PrmVal = variant( PrmPin );

		PrmSet.ParameterNumber = i;

		if( PrmEnt.mType == FF_TYPE_STANDARD )
		{
			PrmSet.NewParameterValue.FloatValue = qBound( 0.0f, PrmVal.value<float>(), 1.0f );

			PMU.PointerValue = &PrmSet;

			MainFunc( FF_SETPARAMETER, PMU, mInstanceId );
		}
	}

	GLint		Viewport[ 4 ];

	glGetIntegerv( GL_VIEWPORT, Viewport );

	GLint		FBOCur;

	glGetIntegerv( GL_FRAMEBUFFER_BINDING, &FBOCur );

	quint32	FBO = DstTex->fbo();

	if( FBO )
	{
		QVector<FFGLTextureStruct>		TexDat( mInputs.size() );
		QVector<FFGLTextureStruct *>	TexPtr;

		for( int i = 0 ; i < mInputs.size() ; i++ )
		{
			fugio::OpenGLTextureInterface	*SrcTex = input<fugio::OpenGLTextureInterface *>( mInputs.at( i ) );

			FFGLTextureStruct	*TexEnt = &TexDat[ i ];

			if( SrcTex )
			{
				TexEnt->Handle = SrcTex->srcTexId();
				TexEnt->HardwareWidth  = SrcTex->textureSize().x();
				TexEnt->HardwareHeight = SrcTex->textureSize().y();
				TexEnt->Width = SrcTex->size().x();
				TexEnt->Height = SrcTex->size().y();
			}
			else
			{
				memset( TexEnt, 0, sizeof( FFGLTextureStruct ) );
			}

			TexPtr << TexEnt;
		}

		glBindFramebuffer( GL_FRAMEBUFFER, FBO );

		glViewport( 0, 0, DstSze.width(), DstSze.height() );

		ProcessOpenGLStruct	PGL;

		PGL.HostFBO          = FBO;
		PGL.numInputTextures = TexPtr.size();
		PGL.inputTextures    = ( TexPtr.isEmpty() ? nullptr : TexPtr.data() );

		PMU.PointerValue = &PGL;

		PMU = MainFunc( FF_PROCESSOPENGL, PMU, mInstanceId );

		glBindFramebuffer( GL_FRAMEBUFFER, FBOCur );

		glViewport( Viewport[ 0 ], Viewport[ 1 ], Viewport[ 2 ], Viewport[ 3 ] );

		for( int i = 0 ; i < mInputs.size() ; i++ )
		{
			glActiveTexture( GL_TEXTURE0 + i );

			fugio::OpenGLTextureInterface	*SrcTex = input<fugio::OpenGLTextureInterface *>( mInputs.at( i ) );

			if( SrcTex )
			{
				SrcTex->release();
			}
		}

		glActiveTexture( GL_TEXTURE0 );
	}

	pinUpdated( mPinOutputTexture );
}

void FFGLNode::render( qint64 pTimeStamp, QUuid pSourcePinId )
{
	fugio::NodeRenderInterface	*ParentRender = input<fugio::NodeRenderInterface *>( mPinInputRender );

	ParentRender->render( pTimeStamp, pSourcePinId );

	initializeOpenGLFunctions();

	GLint		Viewport[ 4 ];

	glGetIntegerv( GL_VIEWPORT, Viewport );


}
