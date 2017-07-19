#ifndef DEVICEOPENGLOUTPUTRPI_H
#define DEVICEOPENGLOUTPUTRPI_H

#include <QWidget>

#if defined( Q_OS_RASPBERRY_PI )
#include "bcm_host.h"
#endif

#include "opengl_includes.h"

#include <fugio/node_interface.h>
#include <fugio/gui/input_events_interface.h>

class DeviceOpenGLOutput : public QWidget
{
	Q_OBJECT

public:
	static void deviceInitialise( void );
	static void deviceDeinitialise( void );
	static void devicePacketStart( qint64 pTimeStamp );
	static void devicePacketEnd( void );

	static QSharedPointer<DeviceOpenGLOutput> newDevice( bool pContextOnly );

signals:
	void cursorMove( const QPointF &pPos );

	void resized( const QSize &pSize );

protected:
	explicit DeviceOpenGLOutput( void );

public:
	virtual ~DeviceOpenGLOutput( void );

	void setCurrentNode( QSharedPointer<fugio::NodeInterface> pNode );
	void unsetCurrentNode( QSharedPointer<fugio::NodeInterface> pNode );

	void renderLater( void );

	bool renderInit( void );

	virtual void renderStart( void );

	virtual void renderEnd( void );

	virtual void processEvents( void )
	{

	}

	virtual QSize windowSize( void ) const;

	virtual QSize framebufferSize( void ) const;

	void setInputEventsInterface( fugio::InputEventsInterface *pInputEvents )
	{
		mInputEvents = pInputEvents;
	}

	inline bool isUpdatePending( void ) const
	{
		return( mUpdatePending );
	}

/*
public:
	virtual QSize windowSize( void ) const Q_DECL_OVERRIDE;

	virtual QSize framebufferSize( void ) const Q_DECL_OVERRIDE;

	virtual bool hasDebugContext( void ) const Q_DECL_OVERRIDE;

public slots:
	virtual void renderStart( void ) Q_DECL_OVERRIDE;

	virtual void renderEnd( void ) Q_DECL_OVERRIDE;

	virtual void processEvents( void ) Q_DECL_OVERRIDE;

	virtual void setGeometry( QRect pRect ) Q_DECL_OVERRIDE;

protected:
	friend class DeviceOpenGLOutput;

	DeviceOpenGLOutputRPi( bool pContextOnly );
mDisplay( 0 ), mConfig( 0 ), mNumConfig( 0 ), mContext( 0 ), mSurface( 0 )
	virtual ~DeviceOpenGLOutputRPi( void );

	virtual void closeOutput() Q_DECL_OVERRIDE;

private:
	*/

protected:
	QWeakPointer<fugio::NodeInterface>			 mNode;

	fugio::InputEventsInterface					*mInputEvents;

	bool										 mUpdatePending;

	EGLDisplay									 mDisplay;
	EGLConfig									 mConfig;
	EGLint										 mNumConfig;
	EGLContext									 mContext;

	EGL_DISPMANX_WINDOW_T						 mNativeWindow;

	DISPMANX_ELEMENT_HANDLE_T					 mDispManElement;
	DISPMANX_DISPLAY_HANDLE_T					 mDispManDisplay;
	DISPMANX_UPDATE_HANDLE_T					 mDispManUpdate;

	VC_RECT_T									 mSrcRct, mDstRct;

	EGLSurface									 mSurface;
};

#endif // DEVICEOPENGLOUTPUTRPI_H
