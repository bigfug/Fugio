#ifndef INTERFACE_DEVICE_FACTORY_H
#define INTERFACE_DEVICE_FACTORY_H

#include <QList>

class QWidget;

#include "global.h"

FUGIO_NAMESPACE_BEGIN

class DeviceFactoryInterface
{
public:
	virtual ~DeviceFactoryInterface( void ) {}

	virtual QString deviceConfigMenuText( void ) const = 0;

	virtual void deviceConfigGui( QWidget *pParent ) = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::DeviceFactoryInterface, "com.bigfug.fugio.device.factory/1.0" )

#endif // INTERFACE_DEVICE_FACTORY_H
