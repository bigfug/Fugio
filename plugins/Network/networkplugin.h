#ifndef NETWORKPLUGIN_H
#define NETWORKPLUGIN_H

#include <fugio/plugin_interface.h>
#include <fugio/device_factory_interface.h>

using namespace fugio;

class NetworkPlugin : public QObject, public PluginInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA( IID "com.bigfug.fugio.network.plugin" )
	Q_INTERFACES( fugio::PluginInterface )

public:
	Q_INVOKABLE explicit NetworkPlugin( void ) : mApp( 0 ) { mInstance = this; }

	virtual ~NetworkPlugin( void ) {}

	static NetworkPlugin *instance( void )
	{
		return( mInstance );
	}

	inline GlobalInterface *app( void )
	{
		return( mApp );
	}

	//-------------------------------------------------------------------------
	// fugio::PluginInterface

	virtual InitResult initialise( fugio::GlobalInterface *pApp );

	virtual void deinitialise( void );

private:
	static NetworkPlugin		*mInstance;

	GlobalInterface				*mApp;
};

#endif // NETWORKPLUGIN_H
