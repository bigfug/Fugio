#ifndef SERIALPLUGIN_H
#define SERIALPLUGIN_H

#include <QObject>

#include <fugio/core/uuid.h>
#include <fugio/global_interface.h>
#include <fugio/plugin_interface.h>
#include <fugio/device_factory_interface.h>

#include "deviceserial.h"

class SerialPlugin : public QObject, public fugio::PluginInterface, public fugio::DeviceFactoryInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::PluginInterface fugio::DeviceFactoryInterface )
	Q_PLUGIN_METADATA( IID "com.bigfug.fugio.serial.plugin" )

public:
	explicit SerialPlugin( void );

	virtual ~SerialPlugin( void );

	//-------------------------------------------------------------------------
	// InterfacePlugin

	virtual InitResult initialise( fugio::GlobalInterface *pApp, bool pLastChance );

	virtual void deinitialise( void );

	//-------------------------------------------------------------------------
	// InterfaceDeviceFactory

	virtual QString deviceConfigMenuText( void ) const
	{
		return( "Serial Devices" );
	}

	virtual void deviceConfigGui( QWidget *pParent );

protected slots:
	void onGlobalFrameStart( qint64 pTimeStamp );
	void onGlobalFrameEnd( void );

	void configLoad( QSettings &pSettings );
	void configSave( QSettings &pSettings ) const;

private:
	static fugio::ClassEntry		 mNodeClasses[];

	fugio::GlobalInterface			*mApp;
};

#endif // SERIALPLUGIN_H
