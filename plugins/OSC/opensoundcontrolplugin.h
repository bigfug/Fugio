#ifndef OPENSOUNDCONTROLPLUGIN_H
#define OPENSOUNDCONTROLPLUGIN_H

#include <QObject>

#include <fugio/core/uuid.h>
#include <fugio/global_interface.h>
#include <fugio/plugin_interface.h>

using namespace fugio;

class OpenSoundControlPlugin : public QObject, public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::PluginInterface )
	Q_PLUGIN_METADATA( IID "com.bigfug.fugio.osc.plugin" )

public:
	explicit OpenSoundControlPlugin( void );

	virtual ~OpenSoundControlPlugin( void );

	static OpenSoundControlPlugin *instance( void );

	GlobalInterface *fugio( void )
	{
		return( mApp );
	}

	//-------------------------------------------------------------------------
	// fugio::PluginInterface

	virtual InitResult initialise( fugio::GlobalInterface *pApp, bool pLastChance );

	virtual void deinitialise( void );

private:
	static ClassEntry				 mNodeClasses[];
	static ClassEntry				 mPinClasses[];

	static OpenSoundControlPlugin	*mInstance;

	GlobalInterface					*mApp;
};

#endif // OPENSOUNDCONTROLPLUGIN_H
