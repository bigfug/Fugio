#include "textnode.h"

#include <QPainter>

#include <fugio/image/uuid.h>
#include <fugio/core/uuid.h>
#include <fugio/painter/uuid.h>

#include <fugio/pin_variant_iterator.h>

TextNode::TextNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_OUTPUT_PAINTER, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_PAINTER, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_INPUT_TEXT, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );
	FUGID( PIN_INPUT_FONT, "6A69B0D7-EE8C-4584-A5F1-6E2178A82AEE" );
	FUGID( PIN_INPUT_POSITION, "09C0F5F7-7ECB-4D2C-AA4D-D101B1DF1385" );
	FUGID( PIN_INPUT_PEN, "4D377F4D-21DA-4C8A-85FC-8E05A4CC1201" );
	FUGID( PIN_INPUT_BRUSH, "94009AFE-CB4C-4088-A1A9-A798970A83EF" );
	FUGID( PIN_INPUT_TRANSFORM, "9A3BBEEB-8F9B-4896-A6A5-FBB9E9DC917F" );

	mPinInputPainter = pinInput( "Painter", PIN_INPUT_PAINTER );

	mValOutputPainter = pinOutput<fugio::PainterInterface *>( "Painter", mPinOutputPainter, PID_PAINTER, PIN_OUTPUT_PAINTER );

	mValOutputPainter->setSource( this );

	mPinInputText = pinInput( "Text", PIN_INPUT_TEXT );

	mPinInputFont = pinInput( "Font", PIN_INPUT_FONT );

	mPinInputPosition = pinInput( "Position", PIN_INPUT_POSITION );

	mPinInputPen = pinInput( "Pen", PIN_INPUT_PEN );

	mPinInputBrush = pinInput( "Brush", PIN_INPUT_BRUSH );

	mPinInputTransform = pinInput( "Transform", PIN_INPUT_TRANSFORM );

	mPinInputPen->setValue( QColor( Qt::white ) );

	mPinInputBrush->setValue( QColor( Qt::transparent ) );

	mPinInputFont->setValue( QFont() );
}

void TextNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	pinUpdated( mPinOutputPainter );
}

void TextNode::paint( QPainter &pPainter, const QRect &pRect )
{
	fugio::PainterInterface		*Painter = input<fugio::PainterInterface *>( mPinInputPainter );

	if( Painter )
	{
		Painter->paint( pPainter, pRect );
	}

	fugio::PinVariantIterator	 Text( mPinInputText );
	fugio::PinVariantIterator	 Font( mPinInputFont );
	fugio::PinVariantIterator	 Position( mPinInputPosition );
	fugio::PinVariantIterator	 Pen( mPinInputPen );
	fugio::PinVariantIterator	 Brush( mPinInputBrush );
	fugio::PinVariantIterator	 Transform( mPinInputTransform );

	if( Text.isEmpty() || Position.isEmpty() )
	{
		return;
	}

	const std::vector<int>		 ItrCnt = { Text.count(), Font.count(), Pen.count(), Brush.count(), Transform.count() };

	auto						 MinMax = std::minmax_element( ItrCnt.begin(), ItrCnt.end() );

	for( int i = 0 ; i < *MinMax.second ; i++ )
	{
		QString		t = Text.index( i ).toString();

		painterSetPen( pPainter, Pen, i );

		painterSetBrush( pPainter, Brush, i );

		pPainter.setTransform( Transform.index( i ).value<QTransform>() );

		pPainter.setFont( Font.index( i ).value<QFont>() );

		QVariant	p = Position.index( i );

		if( QMetaType::Type( p.type() ) == QMetaType::QPoint )
		{
			pPainter.drawText( p.toPoint(), t );
		}
		else if( QMetaType::Type( p.type() ) == QMetaType::QPointF )
		{
			pPainter.drawText( p.toPointF(), t );
		}
		else if( QMetaType::Type( p.type() ) == QMetaType::QRect )
		{
			pPainter.drawText( p.toRect(), t );
		}
		else if( QMetaType::Type( p.type() ) == QMetaType::QRectF )
		{
			pPainter.drawText( p.toRectF(), t );
		}
		else if( QMetaType::Type( p.type() ) == QMetaType::QSize )
		{
			pPainter.drawText( QRect( QPoint(), p.toSize() ), t );
		}
		else if( QMetaType::Type( p.type() ) == QMetaType::QSizeF )
		{
			pPainter.drawText( QRectF( QPointF(), p.toSizeF() ), Qt::AlignHCenter | Qt::AlignCenter, t );
		}
	}
}
