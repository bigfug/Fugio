#ifndef HAND_INTERFACE_H
#define HAND_INTERFACE_H

#if defined( LEAP_PLUGIN_SUPPORTED )
#include <Leap.h>
#endif

#include <fugio/global.h>

FUGIO_NAMESPACE_BEGIN

class HandInterface
{
public:
	virtual ~HandInterface( void ) {}

#if defined( LEAP_PLUGIN_SUPPORTED )
	virtual const Leap::Hand &hand( void ) const = 0;
#endif
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::HandInterface, "com.bigfug.fugio.leap.hand/1.0" )

#endif // HAND_INTERFACE_H

