#ifndef VIDEOCAPTUREPLUGIN_H
#define VIDEOCAPTUREPLUGIN_H

#include <QVector>
#include <QWeakPointer>

#include <fugio/plugin_interface.h>

using namespace fugio;

#include "videocapturedevice.h"

class VideoCapturePlugin : public QObject, public PluginInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA( IID "com.bigfug.fugio.videocapture.plugin" )
	Q_INTERFACES( fugio::PluginInterface )

public:
	Q_INVOKABLE explicit VideoCapturePlugin( void );

	virtual ~VideoCapturePlugin( void ) {}

	static VideoCapturePlugin *instance( void )
	{
		return( mInstance );
	}

	GlobalInterface *app( void )
	{
		return( mApp );
	}

	QSharedPointer<VideoCaptureDevice> device( int pDeviceIndex, int pFormatIndex );

	//-------------------------------------------------------------------------
	// fugio::PluginInterface

	virtual InitResult initialise( fugio::GlobalInterface *pApp, bool pLastChance );

	virtual void deinitialise( void );

private:
	static VideoCapturePlugin					*mInstance;

	GlobalInterface								*mApp;
	QVector<QWeakPointer<VideoCaptureDevice>>	 mDevices;
};

#endif // VIDEOCAPTUREPLUGIN_H
