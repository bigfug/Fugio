#ifndef DEVICEOPENGLOUTPUTRPI_H
#define DEVICEOPENGLOUTPUTRPI_H

#include "deviceopengloutput.h"

#if defined( Q_OS_RASPBERRY_PI )
#include "bcm_host.h"
#endif

class DeviceOpenGLOutputRPi : public DeviceOpenGLOutput
{
	Q_OBJECT

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

	virtual ~DeviceOpenGLOutputRPi( void );

	virtual void closeOutput() Q_DECL_OVERRIDE;

private:
	EGLDisplay							 mDisplay;
	EGLConfig							 mConfig;
	EGLint								 mNumConfig;
	EGLContext	mContext;

	EGL_DISPMANX_WINDOW_T		NativeWindow;

	DISPMANX_ELEMENT_HANDLE_T	DispManElement;
	DISPMANX_DISPLAY_HANDLE_T	DispManDisplay;
	DISPMANX_UPDATE_HANDLE_T	DispManUpdate;

	VC_RECT_T					SrcRct, DstRct;

	EGLSurface		mSurface;
};

#endif // DEVICEOPENGLOUTPUTRPI_H
