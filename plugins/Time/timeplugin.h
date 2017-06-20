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
	Q_INVOKABLE explicit TimePlugin( void );

	virtual ~TimePlugin( void ) {}

	static TimePlugin *instance( void )
	{
		return( mInstance );
	}

	inline GlobalInterface *app( void ) const
	{
		return( mApp );
	}

	//-------------------------------------------------------------------------
	// fugio::PluginInterface

	virtual InitResult initialise( fugio::GlobalInterface *pApp, bool pLastChance );

	virtual void deinitialise( void );

private:
	static TimePlugin			*mInstance;

	GlobalInterface				*mApp;
};

#endif // TIMEPLUGIN_H
