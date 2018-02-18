#include "painternode.h"

#include <QPainter>
#include <QColor>

#include <fugio/core/uuid.h>
#include <fugio/image/image.h>
#include <fugio/image/uuid.h>
#include <fugio/image/image.h>
#include <fugio/pin_variant_iterator.h>
#include <fugio/image/painter_interface.h>
#include <fugio/performance.h>

PainterNode::PainterNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_SIZE, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_PAINTER, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_OUTPUT_IMAGE, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );
	//FUGID( PIN_XXX_XXX, "249f2932-f483-422f-b811-ab679f006381" );
	//FUGID( PIN_XXX_XXX, "ce8d578e-c5a4-422f-b3c4-a1bdf40facdb" );

	mPinInputTrigger = pinInput( "Trigger", PID_FUGIO_NODE_TRIGGER );

	mPinInputSize = pinInput( "Size", PIN_INPUT_SIZE );

	mPinInputSize->registerPinInputType( PID_SIZE );

	mPinInputPainter = pinInput( "Painter", PIN_INPUT_PAINTER );

	mValOutputImage = pinOutput<fugio::VariantInterface *>( "Image", mPinOutputImage, PID_IMAGE, PIN_OUTPUT_IMAGE );
}

void PainterNode::inputsUpdated( qint64 pTimeStamp )
{
	fugio::Performance	P( mNode, __FUNCTION__, pTimeStamp );

	NodeControlBase::inputsUpdated( pTimeStamp );

	QSize			ImgSze = variant( mPinInputSize ).toSize();

	fugio::Image	CurImg = mValOutputImage->variant().value<fugio::Image>();

	if( !ImgSze.isEmpty() && ImgSze != CurImg.size() )
	{
		CurImg.setFormat( fugio::ImageFormat::BGRA8 );
		CurImg.setLineSize( 0, 4 * ImgSze.width() );
		CurImg.setSize( ImgSze.width(), ImgSze.height() );

		QVariant	V;

		V.setValue( CurImg );

		mValOutputImage->setVariant( V );
	}

	if( !ImgSze.isEmpty() && mPinInputPainter->isConnected() )
	{
		if( true )
		{
			QImage		IM( CurImg.internalBuffer( 0 ), ImgSze.width(), ImgSze.height(), CurImg.lineSize( 0 ), QImage::Format_ARGB32_Premultiplied );

			if( true )
			{
				QPainter	Painter( &IM );

				fugio::PainterInterface	*PI = input<fugio::PainterInterface *>( mPinInputPainter );

				if( PI )
				{
					PI->paint( Painter, IM.rect() );
				}
			}
		}

		// The image is stored using a premultiplied 32-bit ARGB format (0xAARRGGBB),
		// i.e. the red, green, and blue channels are multiplied by the alpha component
		// divided by 255.

		for( int y = 0 ; y < ImgSze.height() ; y++ )
		{
			QRgb	*RGB = reinterpret_cast<QRgb *>( CurImg.internalBuffer( 0 ) + CurImg.lineSize( 0 ) );

			for( int x = 0  ; x < ImgSze.width() ; x++, RGB++ )
			{
				*RGB = qUnpremultiply( *RGB );
			}
		}
	}

	pinUpdated( mPinOutputImage );
}

