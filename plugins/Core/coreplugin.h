#ifndef COREPLUGIN_H
#define COREPLUGIN_H

#include <QObject>

#include <fugio/core/uuid.h>
#include <fugio/global_interface.h>
#include <fugio/plugin_interface.h>

using namespace fugio;

class CorePlugin : public QObject, public fugio::PluginInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::PluginInterface )
	Q_PLUGIN_METADATA( IID "com.bigfug.fugio.core.plugin" )

public:
	explicit CorePlugin( void );

	virtual ~CorePlugin( void ) Q_DECL_OVERRIDE {};

	static CorePlugin *instance( void )
	{
		return( mInstance );
	}

	fugio::GlobalInterface *app( void )
	{
		return( mApp );
	}

	//-------------------------------------------------------------------------
	// fugio::PluginInterface

	virtual InitResult initialise( fugio::GlobalInterface *pApp, bool pLastChance ) Q_DECL_OVERRIDE;

	virtual void deinitialise( void ) Q_DECL_OVERRIDE;

private:
	static ClassEntry				 mNodeClasses[];
	static ClassEntry				 mPinClasses[];

	static CorePlugin				*mInstance;
	fugio::GlobalInterface			*mApp;
};

#endif // COREPLUGIN_H
