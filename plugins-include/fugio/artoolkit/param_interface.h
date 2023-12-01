#ifndef PARAM_PIN_INTERFACE_H
#define PARAM_PIN_INTERFACE_H

#include <QtPlugin>

#include <fugio/global.h>

#include <fugio/node_control_interface.h>

#if defined( ARTOOLKIT_SUPPORTED )
#include <AR/ar.h>
#endif

FUGIO_NAMESPACE_BEGIN

namespace ar {

class ParamInterface
{
public:
	virtual ~ParamInterface( void ) {}

#if defined( ARTOOLKIT_SUPPORTED )
	virtual const ARParam &param( void ) const = 0;

	virtual void setParam( const ARParam &pParam ) = 0;
#endif
};

} // namespace ar

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::ar::ParamInterface, "com.bigfug.fugio.ar.param/1.0" )

#endif // PARAM_PIN_INTERFACE_H
