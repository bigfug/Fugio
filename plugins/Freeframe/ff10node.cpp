#include "ff10node.h"

#include <QImage>

#include "freeframeplugin.h"

#include <fugio/context_interface.h>

#include <fugio/image/uuid.h>

FF10Node::FF10Node( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mInstanceId( 0 )
{
	FUGID( PIN_INPUT_IMAGE, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	//FUGID( PIN_XXX_XXX, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	//FUGID( PIN_XXX_XXX, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );

	pinInput( "Trigger", PID_FUGIO_NODE_TRIGGER );

	mValOutput = pinOutput<fugio::VariantInterface *>( "Image", mPinOutput, PID_IMAGE, PIN_INPUT_IMAGE );

	if( ( mLibrary = FreeframePlugin::findPluginInfo( mNode->controlUuid() ) ) )
	{
		if( mLibrary->initialise() )
		{
			//-------------------------------------------------------------------------

			//FF_Main_FuncPtr		mMainFunc = mLibrary->func();

			if( mLibrary->maxInputFrames() > 1 )
			{
				for( int i = 0 ; i < mLibrary->maxInputFrames() ; i++ )
				{
					mInputs.append( pinInput( QString( "Image %1" ).arg( i ), QUuid::createUuid() ) );
				}
			}
			else
			{
				mInputs.append( pinInput( "Image", QUuid::createUuid() ) );
			}

			//-------------------------------------------------------------------------

			for( const FreeframeLibrary::ParamEntry &Param : mLibrary->params() )
			{
//				mParams.append( pinInput( Param, QUuid::createUuid() ) );
			}
		}
	}
}

bool FF10Node::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	return( true );
}

bool FF10Node::deinitialise()
{
	return( true );
}

void FF10Node::inputsUpdated( qint64 pTimeStamp )
{
//	FF_Main_FuncPtr		mMainFunc = mLibrary->func();

//	plugMainUnion		PMU;

//	if( !mInputs.first()->isConnectedToActiveNode() || !mInputs.first()->isUpdated( pTimeStamp ) )
//	{
//		return;
//	}

//	const fugio::Image	SrcImg = variant<fugio::Image>( mInputs.first() );

//	if( !SrcImg.isValid() )
//	{
//		return;
//	}

//	if( SrcImg->size() != mValOutput->size() )
//	{
//		if( mInstanceId != 0 )
//		{
//			PMU = mMainFunc( FF_DEINSTANTIATE, 0, mInstanceId );

//			mInstanceId = 0;
//		}

//		VideoInfoStruct		VIS;

//		VIS.FrameWidth  = SrcImg->size().width();
//		VIS.FrameHeight = SrcImg->size().height();
//		VIS.Orientation = FF_ORIENTATION_TL;

//		switch( SrcImg->format() )
//		{
//			case InterfaceImage::FORMAT_RGB8:
//				VIS.BitDepth = FF_DEPTH_24;
//				break;

//			case InterfaceImage::FORMAT_RGBA8:
//				VIS.BitDepth = FF_DEPTH_32;
//				break;

//			case InterfaceImage::FORMAT_BGR8:
//				VIS.BitDepth = FF_DEPTH_24;
//				break;

//			case InterfaceImage::FORMAT_BGRA8:
//				VIS.BitDepth = FF_DEPTH_32;
//				break;

//			default:
//				return;
//		}

//		PMU = mMainFunc( FF_INSTANTIATE, DWORD( &VIS ), 0 );

//		if( PMU.ivalue == FF_FAIL )
//		{
//			return;
//		}

//		mInstanceId = PMU.ivalue;

//		mValOutput->setFormat( SrcImg->format() );
//		mValOutput->setSize( SrcImg->size().width(), SrcImg->size().height(), SrcImg->lineWidth() );
//	}

//	if( !mInstanceId || mValOutput->size().isEmpty() )
//	{
//		return;
//	}

////	if( mLibrary->hasProcessFrameCopy() )
////	{

////	}
////	else
//	{
//		memcpy( mValOutput->internalBuffer(), SrcImg->buffer(), SrcImg->bufferSize() );

//		mMainFunc( FF_PROCESSFRAME, DWORD( mValOutput->internalBuffer() ), mInstanceId );
//	}

//	pinUpdated( mPinOutput );
}
