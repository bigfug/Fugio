#ifndef ISFPLUGIN_H
#define ISFPLUGIN_H

#include <QObject>
#include <QDir>

#include <fugio/core/uuid.h>
#include <fugio/global_interface.h>
#include <fugio/plugin_interface.h>
#include <fugio/editor_interface.h>

using namespace fugio;

class ISFPlugin : public QObject, public fugio::PluginInterface, public fugio::SettingsInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::PluginInterface fugio::SettingsInterface )
	Q_PLUGIN_METADATA( IID "com.bigfug.fugio.isf.plugin" )

public:
	explicit ISFPlugin( void );

	virtual ~ISFPlugin( void ) {}

	virtual bool hasContext( void );

	static inline ISFPlugin *instance( void )
	{
		return( mInstance );
	}

	static bool hasContextStatic( void );

	inline static fugio::GlobalInterface *app( void )
	{
		return( instance()->mApp );
	}

	QString pluginPath( const QUuid &pUuid ) const
	{
		return( mPluginUuid.value( pUuid ) );
	}

	//-------------------------------------------------------------------------
	// fugio::PluginInterface

public:
	virtual InitResult initialise( fugio::GlobalInterface *pApp, bool pLastChance ) Q_DECL_OVERRIDE;

	virtual void deinitialise( void ) Q_DECL_OVERRIDE;


	//-------------------------------------------------------------------------
	// SettingsInterface interface
public:
	virtual QWidget *settingsWidget() Q_DECL_OVERRIDE;

	virtual void settingsAccept( QWidget *W ) Q_DECL_OVERRIDE;

private:
	static void scanDirectory( ClassEntryList &pEntLst, QDir pDir, QMap<QUuid,QString> &pUuidList, QStringList pPath = QStringList() );

private:
	static ClassEntry				 mNodeClasses[];
	static ClassEntry				 mPinClasses[];

	static ISFPlugin				*mInstance;

	fugio::GlobalInterface			*mApp;

	ClassEntryList					 mSharedClassEntry;
	ClassEntryList					 mPluginClassEntry;

	QMap<QUuid,QString>				 mPluginUuid;
};

#endif // ISFPLUGIN_H
