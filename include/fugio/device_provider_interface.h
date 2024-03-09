#ifndef DEVICE_PROVIDER_INTERFACE_H
#define DEVICE_PROVIDER_INTERFACE_H

#include <QtPlugin>

class QWidget;

#include "global.h"

FUGIO_NAMESPACE_BEGIN

class DeviceProviderInterface
{
public:
	virtual ~DeviceProviderInterface( void ) {}

	virtual QObject *qobject( void ) = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::DeviceProviderInterface, "com.bigfug.fugio.device.provider/1.0" )

#endif // DEVICE_PROVIDER_INTERFACE_H
