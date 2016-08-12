#ifndef LEDITEM_H
#define LEDITEM_H

#include <QGraphicsObject>
#include <QGraphicsEllipseItem>

class LEDItem : public QGraphicsObject
{
	Q_OBJECT

public:
	LEDItem();

	// QGraphicsItem interface
public:
	virtual QRectF boundingRect() const Q_DECL_OVERRIDE;

	virtual void paint( QPainter *pPainter, const QStyleOptionGraphicsItem *pOption, QWidget *pWidget ) Q_DECL_OVERRIDE;

	// QGraphicsItem interface
protected:
	virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;

public slots:
	void setBrightness( double pBrightness );

	void setColour( const QColor &pColor );

signals:
	void colourChanged( const QColor &pColour );

private:
	QGraphicsEllipseItem		*mEllipse;
	double						 mBrightness;
	QColor						 mColour;
};

#endif // LEDITEM_H
