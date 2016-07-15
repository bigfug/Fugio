#ifndef IMAGEPLUGIN_H
#define IMAGEPLUGIN_H

#include <QObject>

#include <fugio/core/uuid.h>
#include <fugio/global_interface.h>
#include <fugio/plugin_interface.h>

using namespace fugio;

class ImagePlugin : public QObject, public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::PluginInterface )
	Q_PLUGIN_METADATA( IID "com.bigfug.fugio.image.plugin" )

public:
	explicit ImagePlugin( void );

	virtual ~ImagePlugin( void ) {}

	//-------------------------------------------------------------------------
	// fugio::PluginInterface

	virtual InitResult initialise( fugio::GlobalInterface *pApp );

	virtual void deinitialise( void );

private:
	fugio::GlobalInterface			*mApp;
};

#endif // IMAGEPLUGIN_H
