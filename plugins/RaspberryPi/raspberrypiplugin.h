#ifndef RASPBERRYPIPLUGIN_H
#define RASPBERRYPIPLUGIN_H

#include <fugio/plugin_interface.h>

using namespace fugio;

class RasperryPiPlugin : public QObject, public fugio::PluginInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA( IID "com.bigfug.fugio.raspberry-pi.plugin" )
	Q_INTERFACES( fugio::PluginInterface )

public:
	Q_INVOKABLE explicit RasperryPiPlugin( void );

	virtual ~RasperryPiPlugin( void ) {}

	//-------------------------------------------------------------------------
	// fugio::PluginInterface

	virtual InitResult initialise( fugio::GlobalInterface *pApp, bool pLastChance );

	virtual void deinitialise( void );

private:
	GlobalInterface			*mApp;

#if defined( PIGPIO_SUPPORTED )
	int						 mPigPioInit;
#endif
};

#endif // RASPBERRYPIPLUGIN_H
