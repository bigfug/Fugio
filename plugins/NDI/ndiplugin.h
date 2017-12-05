#ifndef SFALICPLUGIN_H
#define SFALICPLUGIN_H

#include <fugio/plugin_interface.h>

using namespace fugio;

class NDIPlugin : public QObject, public PluginInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA( IID "com.bigfug.fugio.ndi.plugin" )
	Q_INTERFACES( fugio::PluginInterface )

public:
	Q_INVOKABLE explicit NDIPlugin( void ) {}

	virtual ~NDIPlugin( void ) {}

	//-------------------------------------------------------------------------
	// fugio::PluginInterface

	virtual InitResult initialise( fugio::GlobalInterface *pApp, bool pLastChance );

	virtual void deinitialise( void );

private:
	GlobalInterface			*mApp;
};

#endif // SFALICPLUGIN_H
