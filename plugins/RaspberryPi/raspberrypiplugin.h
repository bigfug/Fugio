#ifndef RASPBERRYPIPLUGIN_H
#define RASPBERRYPIPLUGIN_H

#include <fugio/plugin_interface.h>

using namespace fugio;

class RaspberryPiPlugin : public QObject, public fugio::PluginInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA( IID "com.bigfug.fugio.raspberry-pi.plugin" )
	Q_INTERFACES( fugio::PluginInterface )

public:
	Q_INVOKABLE explicit RaspberryPiPlugin( void );

	virtual ~RaspberryPiPlugin( void ) {}

	static RaspberryPiPlugin *instance( void )
	{
		return( mInstance );
	}

	int piIndex( void ) const
	{
		return( mPigPioInit );
	}

	//-------------------------------------------------------------------------
	// fugio::PluginInterface

	virtual InitResult initialise( fugio::GlobalInterface *pApp, bool pLastChance );

	virtual void deinitialise( void );

private:
	static RaspberryPiPlugin	*mInstance;

	GlobalInterface				*mApp;

	int							 mPigPioInit;


};

#endif // RASPBERRYPIPLUGIN_H
