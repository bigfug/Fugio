#include "circlenode.h"

#include <QPainter>

#include <fugio/image/uuid.h>
#include <fugio/core/uuid.h>
#include <fugio/painter/uuid.h>

#include <fugio/pin_variant_iterator.h>

CircleNode::CircleNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_OUTPUT_PAINTER, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_PAINTER, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_INPUT_CENTER, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );
	FUGID( PIN_INPUT_RADIUS, "249f2932-f483-422f-b811-ab679f006381" );
	FUGID( PIN_INPUT_PEN, "4D377F4D-21DA-4C8A-85FC-8E05A4CC1201" );
	FUGID( PIN_INPUT_BRUSH, "94009AFE-CB4C-4088-A1A9-A798970A83EF" );
	FUGID( PIN_INPUT_TRANSFORM, "9A3BBEEB-8F9B-4896-A6A5-FBB9E9DC917F" );

	mPinInputPainter = pinInput( "Painter", PIN_INPUT_PAINTER );

	mValOutputPainter = pinOutput<fugio::PainterInterface *>( "Painter", mPinOutputPainter, PID_PAINTER, PIN_OUTPUT_PAINTER );

	mValOutputPainter->setSource( this );

	mPinInputCenter = pinInput( "Position", PIN_INPUT_CENTER );

	mPinInputCenter->registerPinInputType( PID_POINT );

	mPinInputRadius = pinInput( "Radius", PIN_INPUT_RADIUS );

	mPinInputRadius->setValue( 10.0f );

	mPinInputPen = pinInput( "Pen", PIN_INPUT_PEN );

	mPinInputBrush = pinInput( "Brush", PIN_INPUT_BRUSH );

	mPinInputTransform = pinInput( "Transform", PIN_INPUT_TRANSFORM );
}

void CircleNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	pinUpdated( mPinOutputPainter );
}

void CircleNode::paint( QPainter &pPainter, const QRect &pRect )
{
	fugio::PainterInterface		*Painter = input<fugio::PainterInterface *>( mPinInputPainter );

	if( Painter )
	{
		Painter->paint( pPainter, pRect );
	}

	fugio::PinVariantIterator	 Center( mPinInputCenter );
	fugio::PinVariantIterator	 Radius( mPinInputRadius );
	fugio::PinVariantIterator	 Pen( mPinInputPen );
	fugio::PinVariantIterator	 Brush( mPinInputBrush );
	fugio::PinVariantIterator	 Transform( mPinInputTransform );

	if( Center.isEmpty() || Radius.isEmpty() )
	{
		return;
	}

	const std::vector<int>		 Cnt = { Center.count(), Radius.count(), Pen.count(), Brush.count(), Transform.count() };

	auto						 MinMax = std::minmax_element( Cnt.begin(), Cnt.end() );

	for( int i = 0 ; i < *MinMax.second ; i++ )
	{
		QVariant	V = Center.index( i );

		qreal	r = Radius.index( i ).toReal();

		painterSetPen( pPainter, Pen, i );

		painterSetBrush( pPainter, Brush, i );

		pPainter.setTransform( Transform.index( i ).value<QTransform>() );

		if( QMetaType::Type( V.type() ) == QMetaType::QPoint )
		{
			pPainter.drawEllipse( QPointF( V.toPoint() ), r, r );
		}
		else if( QMetaType::Type( V.type() ) == QMetaType::QPointF )
		{
			pPainter.drawEllipse( V.toPointF(), r, r );
		}
		else if( QMetaType::Type( V.type() ) == QMetaType::QSize )
		{
			QSize	s = V.toSize();

			pPainter.drawEllipse( s.width(), s.height(), r, r );
		}
		else if( QMetaType::Type( V.type() ) == QMetaType::QSizeF )
		{
			QSizeF	s = V.toSizeF();

			pPainter.drawEllipse( s.width(), s.height(), r, r );
		}
		else if( QMetaType::Type( V.type() ) == QMetaType::QRect )
		{
			QRect	s = V.toRect();

			pPainter.drawEllipse( s );
		}
		else if( QMetaType::Type( V.type() ) == QMetaType::QRectF )
		{
			QRectF	s = V.toRectF();

			pPainter.drawEllipse( s );
		}
	}
}
