#ifndef VIDEOCAPTUREPLUGIN_H
#define VIDEOCAPTUREPLUGIN_H

#include <fugio/plugin_interface.h>

using namespace fugio;

class VideoCapturePlugin : public QObject, public PluginInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA( IID "com.bigfug.fugio.videocapture.plugin" )
	Q_INTERFACES( fugio::PluginInterface )

public:
	Q_INVOKABLE explicit VideoCapturePlugin( void );

	virtual ~VideoCapturePlugin( void ) {}

	//-------------------------------------------------------------------------
	// fugio::PluginInterface

	virtual InitResult initialise( fugio::GlobalInterface *pApp, bool pLastChance );

	virtual void deinitialise( void );

private:
	GlobalInterface			*mApp;
};

#endif // VIDEOCAPTUREPLUGIN_H
