#ifndef GEOMETRYPLUGIN_H
#define GEOMETRYPLUGIN_H

#include <QObject>

#include <fugio/core/uuid.h>
#include <fugio/global_interface.h>
#include <fugio/plugin_interface.h>

using namespace fugio;

class GeometryPlugin : public QObject, public fugio::PluginInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::PluginInterface )
	Q_PLUGIN_METADATA( IID "com.bigfug.fugio.geometry.plugin" )

public:
	explicit GeometryPlugin( void );

	virtual ~GeometryPlugin( void ) {}

	//-------------------------------------------------------------------------
	// fugio::PluginInterface

	virtual InitResult initialise( fugio::GlobalInterface *pApp, bool pLastChance );

	virtual void deinitialise( void );

private:
	static ClassEntry		 mNodeClasses[];
	static ClassEntry		 mPinClasses[];

	fugio::GlobalInterface			*mApp;
};

#endif // GEOMETRYPLUGIN_H
