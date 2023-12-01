#ifndef JSONPLUGIN_H
#define JSONPLUGIN_H

#include <cstdlib>

#include <fugio/plugin_interface.h>

class JsonPlugin : public QObject, public fugio::PluginInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA( IID "com.bigfug.fugio.json.plugin" )
	Q_INTERFACES( fugio::PluginInterface )

public:
	Q_INVOKABLE explicit JsonPlugin( void );

	virtual ~JsonPlugin( void ) {}

	//-------------------------------------------------------------------------
	// fugio::PluginInterface

	virtual InitResult initialise( fugio::GlobalInterface *pApp, bool pLastChance );

	virtual void deinitialise( void );

private:
	fugio::GlobalInterface			*mApp;
};


#endif // JSONPLUGIN_H
