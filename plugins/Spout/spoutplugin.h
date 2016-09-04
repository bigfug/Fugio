#ifndef SPOUTPLUGIN_H
#define SPOUTPLUGIN_H

#include "opengl_includes.h"

#include <fugio/plugin_interface.h>
#include <fugio/device_factory_interface.h>

class SpoutPlugin : public QObject, public fugio::PluginInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA( IID "com.bigfug.fugio.spout.plugin" )
	Q_INTERFACES( fugio::PluginInterface )

public:
	Q_INVOKABLE explicit SpoutPlugin( void );

	virtual ~SpoutPlugin( void ) {}

	static SpoutPlugin *instance( void )
	{
		return( mInstance );
	}

	QStringList receiverList( void ) const
	{
		return( mNameList );
	}

	bool hasOpenGLContext( void );

	static bool hasOpenGLContextStatic( void );

	//-------------------------------------------------------------------------
	// fugio::PluginInterface

	virtual InitResult initialise( fugio::GlobalInterface *pApp, bool pLastChance );

	virtual void deinitialise( void );

signals:
	void receiverList( const QStringList &pNameList );

protected slots:
	void updateReceiverList( void );

private:
	static SpoutPlugin			*mInstance;
	fugio::GlobalInterface		*mApp;
	QStringList					 mNameList;
};

#endif // SPOUTPLUGIN_H
