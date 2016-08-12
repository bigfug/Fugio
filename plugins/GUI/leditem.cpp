#include "leditem.h"

#include <QPen>
#include <QRadialGradient>
#include <QColorDialog>
#include <QDebug>
#include <QDateTime>
#include <QApplication>
#include <QGraphicsSceneMouseEvent>

LEDItem::LEDItem()
	: mBrightness( 1 ), mColour( Qt::red )
{
	// Without setting this flag, double-click won't work

	setFlags( QGraphicsItem::ItemIsSelectable );

	mEllipse = new QGraphicsEllipseItem( QRectF( 0, 0, 32, 32 ), this );

	mEllipse->setPen( QPen( Qt::black, 3 ) );
}

QRectF LEDItem::boundingRect() const
{
	return( mEllipse->boundingRect() );
}

void LEDItem::paint( QPainter *pPainter, const QStyleOptionGraphicsItem *pOption, QWidget *pWidget )
{
	QRadialGradient		Gradient( 10, 10, 22 );

	Gradient.setColorAt( 0.0, Qt::white );
	Gradient.setColorAt( 0.1, mColour.darker( 100 + ( 1.0 - ( mBrightness * 1.00 ) ) * 150.0 ) );
	Gradient.setColorAt( 1.0, mColour.darker( 100 + ( 1.0 - ( mBrightness * 0.75 ) ) * 200.0 ) );

	mEllipse->setBrush( Gradient );

	mEllipse->paint( pPainter, pOption, pWidget );
}

void LEDItem::setBrightness( double pBrightness )
{
	mBrightness = qBound( 0.0, pBrightness, 1.0 );

	update();
}

void LEDItem::setColour( const QColor &pColor )
{
	mColour = pColor;

	update();
}

void LEDItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *pEvent)
{
	QColor		C = QColorDialog::getColor( mColour );

	if( C.isValid() && C != mColour )
	{
		setColour( C );

		emit colourChanged( mColour );
	}

	QGraphicsObject::mouseDoubleClickEvent( pEvent );
}
