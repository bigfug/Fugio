#ifndef LEDWIDGET_H
#define LEDWIDGET_H

#include <QWidget>

class LedWidget : public QWidget
{
	Q_OBJECT

public:
	explicit LedWidget(QWidget *parent = 0);
	~LedWidget();

	// QWidget interface
public:
	virtual QSize sizeHint() const;

	// QWidget interface
protected:
	virtual void paintEvent(QPaintEvent *);
	virtual void mouseDoubleClickEvent(QMouseEvent *);

signals:
	void colourChanged( const QColor &pColour );

public slots:
	void setBrightness( double pBrightness );

	void setColour( const QColor &pColor );

protected:
	double		mBrightness;
	QColor		mColour;

	// QWidget interface
protected:
	virtual void mousePressEvent(QMouseEvent *);
};

#endif // LEDWIDGET_H
