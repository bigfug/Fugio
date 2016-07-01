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

	//-------------------------------------------------------------------------
	// fugio::PluginInterface

	virtual InitResult initialise( fugio::GlobalInterface *pApp );

	virtual void deinitialise( void );

private:
	fugio::GlobalInterface			*mApp;
};

#endif // TEXTPLUGIN_H
