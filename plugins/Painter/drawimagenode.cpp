#include "drawimagenode.h"

#include <QPainter>

#include <fugio/image/uuid.h>
#include <fugio/core/uuid.h>
#include <fugio/painter/uuid.h>

#include <fugio/pin_variant_iterator.h>
#include <fugio/image/image.h>
#include <fugio/image/image.h>

DrawImageNode::DrawImageNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_OUTPUT_PAINTER, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_PAINTER, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_INPUT_IMAGE, "f9ee8c61-c7ec-4bd5-aef2-10dbe4c0cdc4" );
	FUGID( PIN_INPUT_POSITION, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );
	FUGID( PIN_INPUT_SOURCE, "249f2932-f483-422f-b811-ab679f006381" );
	FUGID( PIN_INPUT_TRANSFORM, "9A3BBEEB-8F9B-4896-A6A5-FBB9E9DC917F" );

	mPinInputPainter = pinInput( tr( "Painter" ), PIN_INPUT_PAINTER );

	mValOutputPainter = pinOutput<fugio::PainterInterface *>( "Painter", mPinOutputPainter, PID_PAINTER, PIN_OUTPUT_PAINTER );

	mValOutputPainter->setSource( this );

	mPinInputImage = pinInput( tr( "Image" ), PIN_INPUT_IMAGE );

	mPinInputImage->registerPinInputType( PID_IMAGE );

	mPinInputPosition = pinInput( tr( "Position" ), PIN_INPUT_POSITION );

	mPinInputPosition->registerPinInputType( PID_POINT );

	mPinInputPosition->setValue( QPointF() );

	mPinInputSource = pinInput( "Source", PIN_INPUT_SOURCE );

	mPinInputSource->registerPinInputType( PID_RECT );

	mPinInputSource->setValue( QRectF() );

	mPinInputTransform = pinInput( "Transform", PIN_INPUT_TRANSFORM );
}

void DrawImageNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	pinUpdated( mPinOutputPainter );
}

void DrawImageNode::paint( QPainter &pPainter, const QRect &pRect )
{
	fugio::PainterInterface		*Painter = input<fugio::PainterInterface *>( mPinInputPainter );

	if( Painter )
	{
		Painter->paint( pPainter, pRect );
	}

	fugio::PinVariantIterator	 Image( mPinInputImage );
	fugio::PinVariantIterator	 Position( mPinInputPosition );
	fugio::PinVariantIterator	 Source( mPinInputSource );
	fugio::PinVariantIterator	 Transform( mPinInputTransform );

	if( Image.isEmpty() )
	{
		return;
	}

	const std::vector<int>		 Cnt = { Image.count(), Position.count(), Source.count(), Transform.count() };

	const int					 Max = *std::max_element( Cnt.begin(), Cnt.end() );

	for( int i = 0 ; i < Max ; i++ )
	{
		fugio::Image	FI = Image.index( i ).value<fugio::Image>();
		QImage			I  = FI.image();
		QVariant		PV = Position.index( i );
		QRectF			S  = Source.index( i ).toRectF();

		if( I.isNull() )
		{
			continue;
		}

		pPainter.setTransform( Transform.index( i ).value<QTransform>() );

		if( QMetaType::Type( PV.type() ) == QMetaType::QPoint || QMetaType::Type( PV.type() ) == QMetaType::QPointF )
		{
			QPointF		P = PV.toPointF();

			if( !S.isEmpty() )
			{
				pPainter.drawImage( P, I, S );
			}
			else
			{
				pPainter.drawImage( P, I );
			}
		}
		else if( QMetaType::Type( PV.type() ) == QMetaType::QRect || QMetaType::Type( PV.type() ) == QMetaType::QRectF )
		{
			QRectF		P = PV.toRectF();

			if( !S.isEmpty() )
			{
				pPainter.drawImage( P, I, S );
			}
			else
			{
				pPainter.drawImage( P, I );
			}
		}
	}
}
