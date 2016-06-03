#ifndef TIMEPLUGIN_H
#define TIMEPLUGIN_H

#include <fugio/plugin_interface.h>
#include <fugio/device_factory_interface.h>

using namespace fugio;

class TimePlugin : public QObject, public PluginInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA( IID "com.bigfug.fugio.time.plugin" )
	Q_INTERFACES( fugio::PluginInterface )

public:
	Q_INVOKABLE explicit TimePlugin( void ) : mApp( 0 ) {}

	virtual ~TimePlugin( void ) {}

	//-------------------------------------------------------------------------
	// fugio::PluginInterface

	virtual InitResult initialise( fugio::GlobalInterface *pApp );

	virtual void deinitialise( void );

private:
	GlobalInterface			*mApp;
};

#endif // TIMEPLUGIN_H
