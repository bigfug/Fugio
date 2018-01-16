#ifndef PAINTERWINDOW_H
#define PAINTERWINDOW_H

#include <QWindow>
#include <QPainter>
#include <QBackingStore>

#include <QSharedPointer>

#include <fugio/pin_interface.h>

class PainterWindow : public QWindow
{
	Q_OBJECT
public:
	explicit PainterWindow( QWindow *parent = 0 );

	virtual ~PainterWindow( void ) {}

	virtual void render(QPainter *painter);

public slots:
	void renderLater();
	void renderNow();

	void renderPin( QSharedPointer<fugio::PinInterface> P );

signals:
	void windowUpdated( void );

protected:
	template <class T> T input( QSharedPointer<fugio::PinInterface> &pPin )
	{
		if( !pPin->isConnected() || !pPin->connectedPin()->hasControl() )
		{
			return( 0 );
		}

		return( qobject_cast<T>( pPin->connectedPin()->control()->qobject() ) );
	}

	bool event(QEvent *event) Q_DECL_OVERRIDE;

	void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
	void exposeEvent(QExposeEvent *event) Q_DECL_OVERRIDE;

private:
	QBackingStore *m_backingStore;
	bool m_update_pending;
};

#endif // PAINTERWINDOW_H
