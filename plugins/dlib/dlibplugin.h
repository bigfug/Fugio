#ifndef DLIBPLUGIN_H
#define DLIBPLUGIN_H

#include <fugio/plugin_interface.h>

using namespace fugio;

class DlibPlugin : public QObject, public PluginInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA( IID "com.bigfug.fugio.dlib.plugin" )
	Q_INTERFACES( fugio::PluginInterface )

public:
	Q_INVOKABLE explicit DlibPlugin( void );

	virtual ~DlibPlugin( void ) {}

	//-------------------------------------------------------------------------
	// fugio::PluginInterface

	virtual InitResult initialise( fugio::GlobalInterface *pApp, bool pLastChance );

	virtual void deinitialise( void );

private:
	GlobalInterface			*mApp;
};


#endif // DLIBPLUGIN_H
