#include "dialwidget.h"

#include <QPen>
#include <qmath.h>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QVector2D>

DialWidget::DialWidget()
	: mValue( 0 )
{
	setFlags( QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable );

	mOuter = new QGraphicsEllipseItem( QRectF( 0, 0, 48, 48 ), this );

	mOuter->setPen( QPen( Qt::black, 1 ) );
	mOuter->setBrush( Qt::black );

	mPointer = new QGraphicsRectItem( QRect( -3, 0, 6, 20 ), mOuter );

	mPointer->setPen( QPen( Qt::white ) );
	mPointer->setBrush( Qt::white );

	setValue( 0 );

	setAcceptedMouseButtons( Qt::LeftButton );
}

QRectF DialWidget::boundingRect() const
{
	return( mOuter->boundingRect() );
}

void DialWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	mOuter->paint( painter, option, widget );
}

void DialWidget::setValue( qreal pValue )
{
	pValue = qBound( 0.0, pValue, 1.0 );

	QPointF			C = mOuter->rect().center();
	QTransform		T;

	T.translate( C.x(), C.y() );
	T.rotate( 30.0 + ( 300.0 * pValue ) );

	mPointer->setTransform( T );
}

void DialWidget::mousePressEvent( QGraphicsSceneMouseEvent *pEvent )
{
	if( pEvent->button() == Qt::LeftButton )
	{
		mouseMoveEvent( pEvent );

		pEvent->accept();
	}
}

void DialWidget::mouseMoveEvent( QGraphicsSceneMouseEvent *pEvent )
{
	QPointF			C = mOuter->rect().center();
	QVector2D		V = QVector2D( pEvent->pos() - C ).normalized();
	QVector2D		Y( 0, -1 );
	qreal			D = QVector2D::dotProduct( Y, V );
	qreal			A = qRadiansToDegrees( qAcos( D ) ) * ( V.x() < 0 ? -1 : 1 );
	qreal			B = qBound( -150.0, A, 150.0 );
	qreal			Value = ( B + 150.0 ) / 300.0;

	if( Value != mValue )
	{
		setValue( Value );

		emit valueChanged( Value );

		mValue = Value;
	}
}

void DialWidget::mouseReleaseEvent( QGraphicsSceneMouseEvent *pEvent )
{
	if( pEvent->button() == Qt::LeftButton )
	{
		mouseMoveEvent( pEvent );
	}
}
