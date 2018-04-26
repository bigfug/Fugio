#include "imagesplitnode.h"

#include <fugio/core/uuid.h>
#include <fugio/image/uuid.h>

#include <fugio/image/image.h>

#include <fugio/performance.h>

ImageSplitNode::ImageSplitNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_IMAGE,		"3E9A5B5C-3034-4D52-9F05-5B8E93FAE148" );
	FUGID( PIN_OUTPUT_IMAGE,	"26AE3C1B-5D5C-4DCD-9638-2971CBDF1AC5" );

	mPinInputImage = pinInput( "Image", PIN_INPUT_IMAGE );

	QSharedPointer<fugio::PinInterface>			 PinOutputImage;
	fugio::VariantInterface						*ValOutputImage;

	ValOutputImage = pinOutput<fugio::VariantInterface *>( "Image", PinOutputImage, PID_IMAGE, PIN_OUTPUT_IMAGE );
}

void ImageSplitNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	fugio::Image	SrcImg = variant<fugio::Image>( mPinInputImage );

	if( !SrcImg.isValid() )
	{
		return;
	}

	int				SrcCnt = 0;

	for( int i = 0; i < fugio::ImageData::PLANE_COUNT ; i++ )
	{
		if( SrcImg.bufferSize( i ) > 0 )
		{
			SrcCnt++;
		}
	}

	QList<QSharedPointer<fugio::PinInterface>>	OutPin = mNode->enumOutputPins();

	while( OutPin.size() > SrcCnt )
	{
		mNode->removePin( OutPin.takeLast() );
	}

	while( OutPin.size() < SrcCnt )
	{
		QSharedPointer<fugio::PinInterface>			 PinOutputImage;
		fugio::VariantInterface						*ValOutputImage;

		ValOutputImage = pinOutput<fugio::VariantInterface *>( "Image", PinOutputImage, PID_IMAGE, QUuid::createUuid() );

		OutPin << PinOutputImage;
	}

	for( int i = 0; i < fugio::ImageData::PLANE_COUNT ; i++ )
	{
		if( !SrcImg.bufferSize( i ) )
		{
			continue;
		}

		QSharedPointer<fugio::PinInterface>			 PinOutputImage = OutPin.at( i );
		fugio::VariantInterface						*ValOutputImage = ( PinOutputImage->hasControl() ? qobject_cast<fugio::VariantInterface *>( PinOutputImage->control()->qobject() ) : nullptr );

		if( !ValOutputImage )
		{
			continue;
		}

		fugio::Image		DstImg = ValOutputImage->variant().value<fugio::Image>();

		if( !i )
		{
			DstImg.setSize( SrcImg.size() );
		}
		else
		{
			DstImg.setSize( QSize( SrcImg.width() / 2, SrcImg.height() / 2 ) );
		}

		DstImg.setLineSize( 0, SrcImg.lineSize( i ) );

		const int		SrcLinSiz = SrcImg.lineSize( i );

		if( SrcLinSiz >= DstImg.width() * 2 )
		{
			DstImg.setFormat( fugio::ImageFormat::GRAY16 );
		}
		else if( SrcLinSiz >= DstImg.width() )
		{
			DstImg.setFormat( fugio::ImageFormat::GRAY8 );
		}
		else
		{
			DstImg.setFormat( fugio::ImageFormat::UNKNOWN );
		}

		void				*DstBuf = DstImg.internalBuffer( 0 );

		if( DstBuf )
		{
			memcpy( DstBuf, SrcImg.buffer( i ), SrcImg.bufferSize( i ) );
		}

		pinUpdated( PinOutputImage );
	}
}

