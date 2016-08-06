#ifndef DIALWIDGET_H
#define DIALWIDGET_H

#include <QGraphicsObject>
#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>

class DialWidget : public QGraphicsObject
{
	Q_OBJECT

public:
	DialWidget();

	// QGraphicsItem interface
public:
	virtual QRectF boundingRect() const Q_DECL_OVERRIDE;
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;

	// QGraphicsItem interface
protected:
	virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;

public slots:
	void setValue( qreal pValue );

signals:
	void valueChanged( qreal pValue );

private:
	QGraphicsEllipseItem		*mOuter;
	QGraphicsRectItem			*mPointer;
	qreal						 mValue;
};

#endif // DIALWIDGET_H
