#ifndef XINPUTPLUGIN_H
#define XINPUTPLUGIN_H

#include <fugio/plugin_interface.h>

using namespace fugio;

class XInputPlugin : public QObject, public fugio::PluginInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA( IID "com.bigfug.fugio.xinput.plugin" )
	Q_INTERFACES( fugio::PluginInterface )

public:
	Q_INVOKABLE explicit XInputPlugin( void );

	virtual ~XInputPlugin( void );

	//-------------------------------------------------------------------------
	// fugio::PluginInterface

	virtual InitResult initialise( fugio::GlobalInterface *pApp, bool pLastChance );

	virtual void deinitialise( void );

private slots:
	void onGlobalFrameStart( qint64 pTimeStamp );
//	void onGlobalFrameEnd( qint64 pTimeStamp );

//	void configLoad( QSettings &pSettings );
//	void configSave( QSettings &pSettings ) const;

private:
	GlobalInterface			*mApp;
};


#endif // XINPUTPLUGIN_H
