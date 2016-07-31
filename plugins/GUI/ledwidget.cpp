#include "ledwidget.h"
#include <QPainter>
#include <QColorDialog>
#include <QMouseEvent>
#include <QDebug>

LedWidget::LedWidget(QWidget *parent) : QWidget(parent),
	mBrightness( 0 ), mColour( Qt::blue )
{
}

LedWidget::~LedWidget()
{

}

void LedWidget::paintEvent( QPaintEvent *pEvent )
{
	Q_UNUSED( pEvent )

	QColor			C;
	qreal			B = 0.25 + ( mBrightness * 0.75 );

	C.setRedF( mColour.redF() * B );
	C.setGreenF( mColour.greenF() * B );
	C.setBlueF( mColour.blueF() * B );
	C.setAlphaF( 1.0 );

	QPainter		Painter( this );

	Painter.setPen( QPen( Qt::black, 2 ) );
	Painter.setBrush( C );

	Painter.drawEllipse( rect().adjusted( 2, 2, -2, -2 ) );
}

void LedWidget::setBrightness(double pBrightness)
{
	if( pBrightness == mBrightness )
	{
		return;
	}

	mBrightness = qBound( 0.0, pBrightness, 1.0 );

	update();
}

void LedWidget::setColour(const QColor &pColor)
{
	if( pColor == mColour )
	{
		return;
	}

	mColour = pColor;

	update();
}


QSize LedWidget::sizeHint() const
{
	return( QSize( 32, 32 ) );
}


void LedWidget::mouseDoubleClickEvent(QMouseEvent *)
{
	QColor		C = QColorDialog::getColor( mColour );

	if( C.isValid() && C != mColour )
	{
		mColour = C;

		emit colourChanged( mColour );
	}
}

void LedWidget::mousePressEvent( QMouseEvent *pEvent )
{
	Q_UNUSED( pEvent )
}
