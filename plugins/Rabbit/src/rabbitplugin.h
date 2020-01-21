#ifndef RABBITPLUGIN_H
#define RABBITPLUGIN_H

#include <cstdlib>

#include <fugio/global_interface.h>
#include <fugio/plugin_interface.h>

class RabbitPlugin : public QObject, public fugio::PluginInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::PluginInterface )

	Q_PLUGIN_METADATA( IID "com.bigfug.fugio.rabbit.plugin" FILE "manifest.json" )

public:
	Q_INVOKABLE explicit RabbitPlugin( void );

	virtual ~RabbitPlugin( void ) {}

	//-------------------------------------------------------------------------
	// fugio::PluginInterface

	virtual InitResult initialise( fugio::GlobalInterface *pApp, bool pLastChance );

	virtual void deinitialise( void );

private:
	fugio::GlobalInterface			*mApp;

	static const fugio::ClassEntry	 mNodeClasses[];
	static const fugio::ClassEntry	 mPinClasses[];
};


#endif // RABBITPLUGIN_H
