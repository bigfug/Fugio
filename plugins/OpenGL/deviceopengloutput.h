#ifndef DEVICEOPENGLOUTPUT_H
#define DEVICEOPENGLOUTPUT_H

#include "opengl_includes.h"

#include <QSettings>
#include <QCloseEvent>

#include <QOpenGLContext>
#include <QOpenGLWindow>
#include <QOpenGLDebugLogger>

#include <fugio/node_interface.h>
#include <fugio/opengl/output_interface.h>
#include <fugio/gui/input_events_interface.h>

class DeviceOpenGLOutput : public QOpenGLWindow, public OpenGLOutputInterface
{
	Q_OBJECT
	Q_INTERFACES( OpenGLOutputInterface )

public:
	static void deviceInitialise( void );
	static void deviceDeinitialise( void );
	static void devicePacketStart( qint64 pTimeStamp );
	static void devicePacketEnd( void );

	static QSharedPointer<DeviceOpenGLOutput> newDevice( void );

protected:
	explicit DeviceOpenGLOutput( void );

public:
	virtual ~DeviceOpenGLOutput( void );

	inline bool isUpdatePending( void ) const
	{
		return( mUpdatePending );
	}

	virtual QSize windowSize( void ) const;

	virtual QSize framebufferSize( void ) const;

	virtual bool hasDebugContext( void ) const
	{
		return( false );
	}

	virtual void closeOutput()
	{

	}

	void setInputEventsInterface( fugio::InputEventsInterface *pInputEvents )
	{
		mInputEvents = pInputEvents;
	}

	// QOpenGLWindow interface
protected:
	virtual void initializeGL() Q_DECL_OVERRIDE;
	virtual void resizeGL(int w, int h) Q_DECL_OVERRIDE;
	virtual void paintGL() Q_DECL_OVERRIDE;

signals:
	void cursorMove( const QPointF &pPos );

	void resized( const QSize &pSize );

public slots:
	void renderLater( void );

	bool renderInit( void );

	virtual void renderStart( void );

	virtual void renderEnd( void );

	virtual void processEvents( void )
	{

	}

	void renderNow();

	void setCurrentNode( QSharedPointer<fugio::NodeInterface> pNode );
	void unsetCurrentNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual void setGeometry( QRect pRect );

protected:
	virtual bool event( QEvent *pEvent ) Q_DECL_OVERRIDE;
//	virtual void resizeEvent( QResizeEvent *pEvent );

//	virtual void closeEvent( QCloseEvent *pEvent ) Q_DECL_OVERRIDE;
	virtual void exposeEvent( QExposeEvent *) Q_DECL_OVERRIDE;
	virtual void keyReleaseEvent( QKeyEvent *pEvent ) Q_DECL_OVERRIDE;

protected slots:
	void toggleFullScreen( void );

	void setFullScreen( void );

	void signalUpdate( void );

	void screengrab( void );

	void handleLoggedMessage( const QOpenGLDebugMessage &debugMessage );

protected:
	QWeakPointer<fugio::NodeInterface>				 mNode;

	fugio::InputEventsInterface						*mInputEvents;

	bool											 mUpdatePending;

	QOpenGLDebugLogger								 mDebugLogger;
};

#endif // DEVICEOPENGLOUTPUT_H
