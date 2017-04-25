#ifndef MIDIPLUGIN_H
#define MIDIPLUGIN_H

#include <QSettings>

#include <fugio/plugin_interface.h>
#include <fugio/device_factory_interface.h>

using namespace fugio;

class MidiPlugin : public QObject, public PluginInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA( IID "com.bigfug.fugio.portmidi.plugin" )
	Q_INTERFACES( fugio::PluginInterface )

public:
	Q_INVOKABLE explicit MidiPlugin( void );

	virtual ~MidiPlugin( void ) {}

	static MidiPlugin *instance( void )
	{
		return( mInstance );
	}

	inline GlobalInterface *app( void )
	{
		return( mApp );
	}

	//-------------------------------------------------------------------------
	// fugio::PluginInterface

	virtual InitResult initialise( fugio::GlobalInterface *pApp, bool pLastChance );

	virtual void deinitialise( void );

	//-------------------------------------------------------------------------
	// fugio::DeviceFactoryInterface

	virtual QString deviceConfigMenuText( void ) const
	{
		return( "MIDI Devices" );
	}

private:
	static MidiPlugin		*mInstance;

	GlobalInterface			*mApp;
};

#endif // MIDIPLUGIN_H
