#ifndef PJLINKPLUGIN_H
#define PJLINKPLUGIN_H

#include <QNetworkAccessManager>

#include <fugio/plugin_interface.h>
#include <fugio/global_interface.h>

#include "pjlinkserver.h"

using namespace fugio;

class PJLinkPlugin : public QObject, public PluginInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA( IID "com.bigfug.fugio.pjlink.plugin" )
	Q_INTERFACES( fugio::PluginInterface )

public:
	Q_INVOKABLE explicit PJLinkPlugin( void );

	virtual ~PJLinkPlugin( void ) {}

	static PJLinkPlugin *instance( void )
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

private:
	static PJLinkPlugin				*mInstance;
	static fugio::ClassEntry		 mNodeClasses[];
	static fugio::ClassEntry		 mPinClasses[];

	GlobalInterface					*mApp;
	QNetworkAccessManager			*mNetworkAccessManager;
	PJLinkServer					*mServer;

};

#endif // PJLINKPLUGIN_H
