#include "painterwindow.h"

#include <QResizeEvent>
#include <QApplication>

#include <fugio/image/image.h>
#include <fugio/image/painter_interface.h>
#include <fugio/core/variant_interface.h>

PainterWindow::PainterWindow(QWindow *parent)
	: QWindow(parent)
	, m_update_pending(false)
{
	m_backingStore = new QBackingStore(this);
	create();

	setGeometry(100, 100, 300, 200);

}

void PainterWindow::exposeEvent(QExposeEvent *)
{
	if (isExposed()) {
		renderNow();
	}
}

void PainterWindow::resizeEvent(QResizeEvent *resizeEvent)
{
	m_backingStore->resize(resizeEvent->size());
	if (isExposed())
	{
		renderNow();
	}

	emit windowUpdated();
}

void PainterWindow::renderNow()
{
	if( true )
	{
		return;
	}

	if (!isExposed())
		return;

	QRect rect(0, 0, width(), height());
	m_backingStore->beginPaint(rect);

	QPaintDevice *device = m_backingStore->paintDevice();
	QPainter painter(device);

	painter.fillRect(0, 0, width(), height(), Qt::white);
	render(&painter);

	m_backingStore->endPaint();
	m_backingStore->flush(rect);
}

void PainterWindow::renderPin( QSharedPointer<fugio::PinInterface> P )
{
	if (!isExposed())
		return;

	QRect rect(0, 0, width(), height());
	m_backingStore->beginPaint(rect);

	QPaintDevice *device = m_backingStore->paintDevice();

	QPainter painter(device);

	if( fugio::VariantInterface *I = input<fugio::VariantInterface *>( P ) )
	{
		const QImage		IM = I->variant().value<fugio::Image>().image();

		painter.drawImage( 0, 0, IM );
	}
	else if( fugio::PainterInterface *I = input<fugio::PainterInterface *>( P ) )
	{
		I->paint( painter, rect );
	}

	m_backingStore->endPaint();
	m_backingStore->flush(rect);
}

void PainterWindow::render(QPainter *painter)
{
	painter->drawText(QRectF(0, 0, width(), height()), Qt::AlignCenter, QStringLiteral("QWindow"));
}

void PainterWindow::renderLater()
{
	if (!m_update_pending) {
		m_update_pending = true;
		QCoreApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
	}
}

bool PainterWindow::event(QEvent *event)
{
	if (event->type() == QEvent::UpdateRequest) {
		m_update_pending = false;
		renderNow();
		return true;
	}
	return QWindow::event(event);
}
