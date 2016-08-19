#ifndef FILEPLUGIN_H
#define FILEPLUGIN_H

#include <QObject>

#include <fugio/core/uuid.h>
#include <fugio/global_interface.h>
#include <fugio/plugin_interface.h>

using namespace fugio;

class FilePlugin : public QObject, public fugio::PluginInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::PluginInterface )
	Q_PLUGIN_METADATA( IID "com.bigfug.fugio.file.plugin" )

public:
	explicit FilePlugin( void );

	virtual ~FilePlugin( void ) {}

	//-------------------------------------------------------------------------
	// fugio::PluginInterface

	virtual InitResult initialise( fugio::GlobalInterface *pApp, bool pLastChance );

	virtual void deinitialise( void );

private:
	GlobalInterface			*mApp;
};

#endif // FILEPLUGIN_H
