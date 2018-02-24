#include "pinpopupfloat.h"

#include <QPen>
#include <QBrush>
#include <QEvent>
#include <QGraphicsScene>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>

#include <fugio/pin_signals.h>
#include <fugio/node_interface.h>
#include <fugio/context_interface.h>

PinPopupFloat::PinPopupFloat( QPointF pMousePosition, QSharedPointer<fugio::PinInterface> P )
	: mPin( P ), mCenter( pMousePosition )
{
	qDebug() << "PinPopupFloat";

	connect( P->qobject(), SIGNAL(displayLabelChanged(QString)), this, SLOT(displayLabelChanged(QString)) );
}

void PinPopupFloat::mouseMoveEvent( QGraphicsSceneMouseEvent *event )
{
	QRectF		R = boundingRect();
	qreal		Value = ( ( event->scenePos().x() - R.x() ) / R.width() );

	mPin->setValue( qBound( 0.0, Value, 1.0 ) );

	fugio::NodeInterface	*N = mPin->node();

	if( N )
	{
		N->context()->updateNode( N->context()->findNode( N->uuid() ) );
	}
}

void PinPopupFloat::mouseReleaseEvent( QGraphicsSceneMouseEvent *event )
{
	deleteLater();

	qDebug() << "mouseReleaseEvent";
}

bool PinPopupFloat::sceneEvent(QEvent *event)
{
	if( event->type() == QEvent::GrabMouse )
	{
		qDebug() << "QEvent::GrabMouse";
	}

	if( event->type() == QEvent::UngrabMouse )
	{
		qDebug() << "QEvent::UngrabMouse";

		deleteLater();

		return( true );
	}

	return( QGraphicsObject::sceneEvent( event ) );
}

void PinPopupFloat::displayLabelChanged( QString pDisplayLabel )
{
	qDebug() << pDisplayLabel;

	mLabel = pDisplayLabel;

	update();
}

QRectF PinPopupFloat::boundingRect() const
{
	QRectF		R( 0, 0, 100, 50 );

	R.moveTopLeft( mCenter - QPointF( R.width() / 2, R.height() / 2 ) );

	return( R );
}

void PinPopupFloat::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
	Q_UNUSED( option )
	Q_UNUSED( widget )

	painter->setPen( Qt::black );
	painter->setBrush( Qt::white );

	painter->drawRect( boundingRect() );

	if( !mLabel.isEmpty() )
	{
		painter->drawText( boundingRect(), Qt::AlignCenter, mLabel );
	}
	else
	{
		painter->drawText( boundingRect(), Qt::AlignCenter, mPin->value().toString() );
	}
}
