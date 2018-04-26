#include "rectnode.h"

#include <QPainter>

#include <fugio/image/uuid.h>
#include <fugio/core/uuid.h>
#include <fugio/painter/uuid.h>

#include <fugio/pin_variant_iterator.h>

RectNode::RectNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_OUTPUT_PAINTER, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_PAINTER, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_INPUT_RECT, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );
	FUGID( PIN_INPUT_PEN, "4D377F4D-21DA-4C8A-85FC-8E05A4CC1201" );
	FUGID( PIN_INPUT_BRUSH, "94009AFE-CB4C-4088-A1A9-A798970A83EF" );
	FUGID( PIN_INPUT_TRANSFORM, "9A3BBEEB-8F9B-4896-A6A5-FBB9E9DC917F" );

	mPinInputPainter = pinInput( "Painter", PIN_INPUT_PAINTER );

	mValOutputPainter = pinOutput<fugio::PainterInterface *>( "Painter", mPinOutputPainter, PID_PAINTER, PIN_OUTPUT_PAINTER );

	mValOutputPainter->setSource( this );

	mPinInputRect = pinInput( "Rect", PIN_INPUT_RECT );

	mPinInputRect->registerPinInputType( PID_RECT );

	mPinInputPen = pinInput( "Pen", PIN_INPUT_PEN );

	mPinInputBrush = pinInput( "Brush", PIN_INPUT_BRUSH );

	mPinInputTransform = pinInput( "Transform", PIN_INPUT_TRANSFORM );
}

void RectNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	pinUpdated( mPinOutputPainter );
}

void RectNode::paint( QPainter &pPainter, const QRect &pRect )
{
	fugio::PainterInterface		*Painter = input<fugio::PainterInterface *>( mPinInputPainter );

	if( Painter )
	{
		Painter->paint( pPainter, pRect );
	}

	fugio::PinVariantIterator	 Rect( mPinInputRect );
	fugio::PinVariantIterator	 Pen( mPinInputPen );
	fugio::PinVariantIterator	 Brush( mPinInputBrush );
	fugio::PinVariantIterator	 Transform( mPinInputTransform );

	if( !Rect.count() )
	{
		return;
	}

	const std::vector<int>		 ItrCnt = { Rect.count(), Pen.count(), Brush.count(), Transform.count() };

	auto						 MinMax = std::minmax_element( ItrCnt.begin(), ItrCnt.end() );

	for( int i = 0 ; i < *MinMax.second ; i++ )
	{
		QRectF	r = Rect.index( i ).toRectF();

		painterSetPen( pPainter, Pen, i );

		painterSetBrush( pPainter, Brush, i );

		pPainter.setTransform( Transform.index( i ).value<QTransform>() );

		pPainter.drawRect( r );
	}
}
