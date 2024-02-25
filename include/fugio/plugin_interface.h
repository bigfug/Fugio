#ifndef INTERFACE_PLUGIN_H
#define INTERFACE_PLUGIN_H

#include <QtPlugin>
#include <fugio/global.h>

FUGIO_NAMESPACE_BEGIN

class GlobalInterface;

class PluginInterface
{
public:
	virtual ~PluginInterface( void ) {}

    typedef enum InitResult
    {
        INIT_OK,
        INIT_FAILED,
        INIT_DEFER
    } InitResult;

	virtual InitResult initialise( fugio::GlobalInterface *pApp, bool pLastChance ) = 0;

    virtual void deinitialise( void ) = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::PluginInterface, "com.bigfug.fugio.plugin/1.0" )

#endif // INTERFACE_PLUGIN_H
