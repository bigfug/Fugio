#ifndef SYPHONPLUGIN_H
#define SYPHONPLUGIN_H

#include <fugio/plugin_interface.h>
#include <fugio/device_factory_interface.h>

using namespace fugio;

class SyphonPlugin : public QObject, public fugio::PluginInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA( IID "com.bigfug.fugio.syphon.plugin" )
	Q_INTERFACES( fugio::PluginInterface )

public:
	Q_INVOKABLE explicit SyphonPlugin( void );

	virtual ~SyphonPlugin( void ) {}

	static SyphonPlugin *instance( void )
	{
		return( mInstance );
	}

	bool hasOpenGLContext( void );

	//-------------------------------------------------------------------------
	// fugio::PluginInterface

	virtual InitResult initialise( fugio::GlobalInterface *pApp, bool pLastChance );

	virtual void deinitialise( void );

private:
	static SyphonPlugin		*mInstance;
	fugio::GlobalInterface	*mApp;
};


#endif // SYPHONPLUGIN_H
