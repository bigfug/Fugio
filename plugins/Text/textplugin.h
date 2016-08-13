#ifndef TEXTPLUGIN_H
#define TEXTPLUGIN_H

#include <QObject>

#include <fugio/core/uuid.h>
#include <fugio/global_interface.h>
#include <fugio/plugin_interface.h>

using namespace fugio;

class TextPlugin : public QObject, public fugio::PluginInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::PluginInterface )
	Q_PLUGIN_METADATA( IID "com.bigfug.fugio.text.plugin" )

public:
	explicit TextPlugin( void );

	virtual ~TextPlugin( void ) {}

	static fugio::GlobalInterface *global( void )
	{
		return( mApp );
	}

	//-------------------------------------------------------------------------
	// fugio::PluginInterface

	virtual InitResult initialise( fugio::GlobalInterface *pApp, bool pLastChance );

	virtual void deinitialise( void );

private:
	static fugio::GlobalInterface	*mApp;
};

#endif // TEXTPLUGIN_H
