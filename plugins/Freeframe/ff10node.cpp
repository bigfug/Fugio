#include "ff10node.h"

#include <QImage>
#include <QCryptographicHash>
#include <QtConcurrent/QtConcurrentRun>

#include "freeframeplugin.h"

#include <fugio/context_interface.h>

#include <fugio/image/uuid.h>

#include <fugio/performance.h>

FF10Node::FF10Node( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mInstanceId( 0 )
{
	FUGID( PIN_OUTPUT_IMAGE, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	//FUGID( PIN_XXX_XXX, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	//FUGID( PIN_XXX_XXX, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );

	pinInput( "Trigger", PID_FUGIO_NODE_TRIGGER );

	mValOutput = pinOutput<fugio::VariantInterface *>( "Image", mPinOutput, PID_IMAGE, PIN_OUTPUT_IMAGE );

	QUuid		InputUuid( "{6618BE23-94BD-4706-817A-6AB4736924D9}" );
	QUuid		ParamUuid( "{973ADEB3-F633-4AFA-B67E-1153DB0069BB}" );

	if( ( mLibrary = FreeframePlugin::findPluginInfo( mNode->controlUuid() ) ) )
	{
		if( mLibrary->initialise() )
		{
			if( mLibrary->maxInputFrames() > 1 )
			{
				for( int i = 0 ; i < mLibrary->maxInputFrames() ; i++ )
				{
					mInputs.append( pinInput( QString( "Image %1" ).arg( i ), InputUuid ) );

					InputUuid = QUuid::fromRfc4122( QCryptographicHash::hash( InputUuid.toRfc4122(), QCryptographicHash::Md5 ) );
				}
			}
			else
			{
				mInputs.append( pinInput( "Image", InputUuid ) );
			}

			//-------------------------------------------------------------------------

			for( const FreeframeLibrary::ParamEntry &PrmEnt : mLibrary->params() )
			{
				QSharedPointer<fugio::PinInterface>		PrmPin = pinInput( PrmEnt.mName, ParamUuid );

				PrmPin->setValue( PrmEnt.mDefault );

				mParams << PrmPin;

				ParamUuid = QUuid::fromRfc4122( QCryptographicHash::hash( ParamUuid.toRfc4122(), QCryptographicHash::Md5 ) );

				PrmPin->setUpdatable( false );
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

#if defined( Q_PROCESSOR_X86_32 )
typedef FFMixed plugMainUnion;
#define ivalue UIntValue
#define svalue PointerValue
#define fvalue FloatValue
#define PISvalue PointerValue
#define VISvalue PointerValue
#endif

void FF10Node::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	mNode->context()->futureSync( QtConcurrent::run( &FF10Node::processStatic, this, pTimeStamp ) );
}

void FF10Node::processStatic( FF10Node *pNode, qint64 pTimeStamp )
{
	pNode->process( pTimeStamp );
}

void FF10Node::process( qint64 pTimeStamp )
{
	fugio::Performance		Perf( mNode, __FUNCTION__, pTimeStamp );

	FF_Main_FuncPtr		MainFunc = mLibrary->func();

	if( !MainFunc )
	{
		return;
	}

	fugio::Image	DstImg = mValOutput->variant().value<fugio::Image>();

	if( DstImg.format() == fugio::ImageFormat::UNKNOWN )
	{
/*		if( mLibrary->flags().testFlag( FreeframeLibrary::CAP_32BIT ) )
		{
			DstImg.setFormat( fugio::ImageFormat::RGBA8 );

//			mBitDepth = FF_CAP_32BITVIDEO;
		}
		else */if( mLibrary->flags().testFlag( FreeframeLibrary::CAP_24BIT ) )
		{
#if defined( Q_OS_WIN )
			DstImg.setFormat( fugio::ImageFormat::BGR8 );
#else
			DstImg.setFormat( fugio::ImageFormat::RGB8 );
#endif

			mBitDepth = FF_CAP_24BITVIDEO;
		}
		else if( mLibrary->flags().testFlag( FreeframeLibrary::CAP_16BIT ) )
		{
			DstImg.setFormat( fugio::ImageFormat::RGB_565 );

			mBitDepth = FF_CAP_16BITVIDEO;
		}
	}

	if( DstImg.format() == fugio::ImageFormat::UNKNOWN )
	{
		return;
	}

	//-------------------------------------------------------------------------
	// Calculate the input size

	QSize		SrcSze;

	for( int i = 0 ; i < mInputs.size() ; i++ )
	{
		fugio::Image	SrcImg = variant<fugio::Image>( mInputs.at( i ) );

		if( !SrcImg.isValid() )
		{
			continue;
		}

		if( SrcImg.width() > SrcSze.width() )
		{
			SrcSze.setWidth( SrcImg.width() );
		}

		if( SrcImg.height() > SrcSze.height() )
		{
			SrcSze.setHeight( SrcImg.height() );
		}

		if( SrcImg.format() != DstImg.format() )
		{
			continue;
		}
	}

	if( !SrcSze.isValid() )
	{
		return;
	}

	//-------------------------------------------------------------------------
	// Initialise the instance

	FFMixed			PMU;

	if( DstImg.size() != SrcSze )
	{
		if( mInstanceId )
		{
			PMU.UIntValue = 0;

			PMU = MainFunc( FF_DEINSTANTIATE, PMU, mInstanceId );

			mInstanceId = 0;
		}

		DstImg.setSize( SrcSze );

		if( mBitDepth == FF_CAP_16BITVIDEO )
		{
			DstImg.setLineSize( 0, 2 * SrcSze.width() );
		}
		else if( mBitDepth == FF_CAP_24BITVIDEO )
		{
			DstImg.setLineSize( 0, 3 * SrcSze.width() );
		}
		else if( mBitDepth == FF_CAP_32BITVIDEO )
		{
			DstImg.setLineSize( 0, 4 * SrcSze.width() );
		}

		mDstBuf.resize( 1024 + DstImg.bufferSize( 0 ) + 1024 );

		DstImg.setBuffer( 0, &mDstBuf[ 1024 ] );

		VideoInfoStruct	VIS;

		VIS.BitDepth    = mBitDepth;
		VIS.FrameWidth  = SrcSze.width();
		VIS.FrameHeight = SrcSze.height();
		VIS.Orientation = FF_ORIENTATION_TL;

		PMU.PointerValue = &VIS;

		PMU = MainFunc( FF_INSTANTIATE, PMU, 0 );

		if( PMU.UIntValue == FF_FAIL )
		{
			return;
		}

		mInstanceId = PMU.PointerValue;
	}

	if( !mInstanceId )
	{
		return;
	}

	//-------------------------------------------------------------------------
	// Prepare the source frames

	QVector<void *>		SrcPtr;

	for( int i = 0 ; i < mInputs.size() ; i++ )
	{
		fugio::Image	SrcImg = variant<fugio::Image>( mInputs.at( i ) );

		if( !SrcImg.isValid() )
		{
			continue;
		}

		if( SrcImg.size() != SrcSze || SrcImg.lineSize( 0 ) != DstImg.lineSize( 0 ) )
		{
			continue;
		}

		SrcPtr << SrcImg.buffer( 0 );
	}

	//-------------------------------------------------------------------------
	// Update the parameters

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
		else if( PrmEnt.mType == FF_TYPE_BOOLEAN )
		{
			PrmSet.NewParameterValue.UIntValue = PrmVal.value<bool>() ? FF_TRUE : FF_FALSE;

			PMU.PointerValue = &PrmSet;

			MainFunc( FF_SETPARAMETER, PMU, mInstanceId );
		}

		PMU.UIntValue = i;

		PMU = MainFunc( FF_GETPARAMETERDISPLAY, PMU, mInstanceId );

		if( PMU.UIntValue != FF_FAIL )
		{
			PrmPin->setDisplayLabel( QString::fromLatin1( QByteArray( (const char *)PMU.PointerValue, 16 ) ) );
		}
	}

	//-------------------------------------------------------------------------
	// Call the plugin

	if( SrcPtr.size() >= mLibrary->minInputFrames() )
	{
		if( mLibrary->hasProcessFrameCopy() )
		{
			ProcessFrameCopyStruct	PFC;

			PFC.numInputFrames = SrcPtr.size();
			PFC.pOutputFrame   = DstImg.buffer( 0 );
			PFC.ppInputFrames  = SrcPtr.data();

			PMU.PointerValue = &PFC;

			PMU = MainFunc( FF_PROCESSFRAMECOPY, PMU, mInstanceId );
		}
		else
		{
			if( !SrcPtr.isEmpty() )
			{
				memcpy( DstImg.buffer( 0 ), SrcPtr.first(), DstImg.bufferSize( 0 ) );
			}

			PMU.PointerValue = DstImg.buffer( 0 );

			PMU = MainFunc( FF_PROCESSFRAME, PMU, mInstanceId );
		}
	}

	//-------------------------------------------------------------------------
	// Update the pin if we succeed

	if( PMU.UIntValue == FF_SUCCESS )
	{
		pinUpdated( mPinOutput );
	}
}
