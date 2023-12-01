#ifndef TUIOPLUGIN_H
#define TUIOPLUGIN_H

#include <QObject>

#include <fugio/core/uuid.h>
#include <fugio/global_interface.h>
#include <fugio/plugin_interface.h>
#include <fugio/nodecontrolbase.h>

using namespace fugio;

class TuioPlugin : public QObject, public fugio::PluginInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::PluginInterface )
	Q_PLUGIN_METADATA( IID "com.bigfug.fugio.tuio.plugin" )

public:
	explicit TuioPlugin( void );

	virtual ~TuioPlugin( void );

	//-------------------------------------------------------------------------
	// fugio::PluginInterface

	virtual InitResult initialise( fugio::GlobalInterface *pApp, bool pLastChance );

	virtual void deinitialise( void );

private:
	GlobalInterface			*mApp;
};

#endif // TUIOPLUGIN_H
