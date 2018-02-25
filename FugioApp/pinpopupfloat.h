#ifndef PINPOPUPFLOAT_H
#define PINPOPUPFLOAT_H

#include <QGraphicsObject>
#include <QSharedPointer>

#include <fugio/pin_interface.h>

class PinPopupFloat : public QGraphicsObject
{
	Q_OBJECT

public:
	PinPopupFloat( QPointF pMousePosition, QSharedPointer<fugio::PinInterface> P );

	// QGraphicsItem interface
protected:
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;

	// QGraphicsItem interface
protected:
	virtual bool sceneEvent(QEvent *event) Q_DECL_OVERRIDE;

protected slots:
	void displayLabelChanged( QString pDisplayLabel );

protected:
	QSharedPointer<fugio::PinInterface>		 mPin;
	QPointF									 mCenter;

	// QGraphicsItem interface
public:
	virtual QRectF boundingRect() const Q_DECL_OVERRIDE;
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;
};

#endif // PINPOPUPFLOAT_H
