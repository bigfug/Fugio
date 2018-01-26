#ifndef VSTPLUGIN_H
#define VSTPLUGIN_H

#include <QObject>
#include <QDir>
#include <QLibrary>
#include <QtPlugin>
#include <QDebug>

#include <fugio/core/uuid.h>
#include <fugio/global_interface.h>
#include <fugio/plugin_interface.h>
#include <fugio/nodecontrolbase.h>

class VSTPlugin : public QObject, public fugio::PluginInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::PluginInterface )
	Q_PLUGIN_METADATA( IID "com.bigfug.fugio.vst.plugin" )

public:
	explicit VSTPlugin( void );

	virtual ~VSTPlugin( void );

	static bool findPluginInfo( const QUuid &pUuid, QString &pLibNam, int &pClsIdx );

	//-------------------------------------------------------------------------
	// InterfacePlugin

	virtual InitResult initialise( fugio::GlobalInterface *pApp, bool pLastChance );

	virtual void deinitialise( void );

protected:
	void pluginDirScan( QDir pDir );

	bool pluginProcess3( QLibrary &pPlgLib );

//	bool pluginProcess2( QLibrary &pPlgLib );

private:
	fugio::GlobalInterface				*mApp;
	fugio::ClassEntryList				 mNodeClasses;
	static QStringList					 mLibraryList;
	static QMap<QUuid,QPair<int,int>>	 mPluginMap;
};

#endif // VSTPLUGIN_H
