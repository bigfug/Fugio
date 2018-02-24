#ifndef FREEFRAMEPLUGIN_H
#define FREEFRAMEPLUGIN_H

#include <fugio/global_interface.h>
#include <fugio/plugin_interface.h>
#include <fugio/editor_interface.h>

#include "freeframelibrary.h"

class QDir;
class QLibrary;

using namespace fugio;

class FreeframePlugin : public QObject, public fugio::PluginInterface, public fugio::SettingsInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA( IID "com.bigfug.fugio.freeframe.plugin" )
	Q_INTERFACES( fugio::PluginInterface fugio::SettingsInterface )

public:
	Q_INVOKABLE explicit FreeframePlugin( void ) : mApp( 0 ) {}

	virtual ~FreeframePlugin( void ) {}

	static FreeframeLibrary *findPluginInfo( const QUuid &pUuid );

	//-------------------------------------------------------------------------
	// InterfacePlugin

	virtual InitResult initialise( fugio::GlobalInterface *pApp, bool pLastChance ) Q_DECL_OVERRIDE;

	virtual void deinitialise( void ) Q_DECL_OVERRIDE;

	//-------------------------------------------------------------------------
	// SettingsInterface interface
public:
	virtual QWidget *settingsWidget() Q_DECL_OVERRIDE;
	virtual void settingsAccept(QWidget *pWidget) Q_DECL_OVERRIDE;

protected:
	void pluginDirScan( QDir pDir );
	void pluginProcess( QLibrary &pPlgLib );

	void reloadPlugins( void );

	void loadPluginPaths();

private:
	fugio::GlobalInterface					*mApp;
	fugio::ClassEntryList					 mPluginClasses;
	QStringList								 mPluginPaths;

	static QMap<QUuid,FreeframeLibrary*>	 mPluginMap;
};

#endif // FREEFRAMEPLUGIN_H
