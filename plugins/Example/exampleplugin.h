#ifndef EXAMPLEPLUGIN_H
#define EXAMPLEPLUGIN_H

#include <QObject>

#include <fugio/core/uuid.h>
#include <fugio/global_interface.h>
#include <fugio/plugin_interface.h>
#include <fugio/nodecontrolbase.h>

using namespace fugio;

class ExamplePlugin : public QObject, public fugio::PluginInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::PluginInterface )
	Q_PLUGIN_METADATA( IID "com.bigfug.fugio.example.plugin" )

public:
	explicit ExamplePlugin( void );

	virtual ~ExamplePlugin( void );

	//-------------------------------------------------------------------------
	// fugio::PluginInterface

	virtual InitResult initialise( fugio::GlobalInterface *pApp );

	virtual void deinitialise( void );

private:
	GlobalInterface			*mApp;
};


#endif // EXAMPLEPLUGIN_H
