#ifndef PORTAUDIOPLUGIN_H
#define PORTAUDIOPLUGIN_H

#include <QObject>

#include <fugio/global_interface.h>
#include <fugio/plugin_interface.h>
#include <fugio/device_factory_interface.h>
#include <fugio/portaudio/uuid.h>

using namespace fugio;

class PortAudioPlugin : public QObject, public PluginInterface, public DeviceFactoryInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::PluginInterface fugio::DeviceFactoryInterface )
	Q_PLUGIN_METADATA( IID "com.bigfug.fugio.portaudio.plugin" )

public:
	explicit PortAudioPlugin( void );

	virtual ~PortAudioPlugin( void );

	static PortAudioPlugin *instance( void )
	{
		return( mInstance );
	}

	GlobalInterface *fugio( void )
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
		return( "PortAudio" );
	}

	virtual void deviceConfigGui( QWidget *pParent );

	//-------------------------------------------------------------------------

private:
	GlobalInterface			*mApp;

	static ClassEntry		 mNodeClasses[];
	static ClassEntry		 mPinClasses[];

	static PortAudioPlugin	*mInstance;
};

#endif // PORTAUDIOPLUGIN_H
