#ifndef NETWORKPLUGIN_H
#define NETWORKPLUGIN_H

#include <fugio/plugin_interface.h>
#include <fugio/device_factory_interface.h>

#include <QNetworkAccessManager>

using namespace fugio;

class NetworkPlugin : public QObject, public PluginInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA( IID "com.bigfug.fugio.network.plugin" )
	Q_INTERFACES( fugio::PluginInterface )

public:
	Q_INVOKABLE explicit NetworkPlugin( void );

	virtual ~NetworkPlugin( void ) {}

	static NetworkPlugin *instance( void )
	{
		return( mInstance );
	}

	inline GlobalInterface *app( void )
	{
		return( mApp );
	}

	static QNetworkAccessManager *nam( void )
	{
		return( mInstance->mNetworkAccessManager );
	}

	//-------------------------------------------------------------------------
	// fugio::PluginInterface

	virtual InitResult initialise( fugio::GlobalInterface *pApp, bool pLastChance );

	virtual void deinitialise( void );

private slots:
	void menuNetworkInformation( void );

private:
	static NetworkPlugin		*mInstance;

	GlobalInterface				*mApp;
	QNetworkAccessManager		*mNetworkAccessManager;
};

#endif // NETWORKPLUGIN_H
