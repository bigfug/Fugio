#ifndef PORTMIDIPLUGIN_H
#define PORTMIDIPLUGIN_H

#include <fugio/plugin_interface.h>

#include "devicemidi.h"

using namespace fugio;

class PortMidiPlugin : public QObject, public PluginInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA( IID "com.bigfug.fugio.portmidi.plugin" )
	Q_INTERFACES( fugio::PluginInterface )

public:
	Q_INVOKABLE explicit PortMidiPlugin( void );

	virtual ~PortMidiPlugin( void ) {}

	static GlobalInterface *app( void )
	{
		return( mInstance->mApp );
	}

	//-------------------------------------------------------------------------
	// fugio::PluginInterface

	virtual InitResult initialise( fugio::GlobalInterface *pApp, bool pLastChance );

	virtual void deinitialise( void );

private slots:
	void onGlobalFrameInitialise( void );

	void onGlobalFrameEnd( void );

private:
	static PortMidiPlugin		*mInstance;

	GlobalInterface				*mApp;
};

#endif // PORTMIDIPLUGIN_H
