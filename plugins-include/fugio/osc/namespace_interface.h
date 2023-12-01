#ifndef NAMESPACE_INTERFACE_H
#define NAMESPACE_INTERFACE_H

#include <QtPlugin>

#include <fugio/global.h>
#include <fugio/node_control_interface.h>

FUGIO_NAMESPACE_BEGIN

namespace osc {

class NamespaceInterface
{
public:
	virtual ~NamespaceInterface( void ) {}

	virtual QStringList oscNamespace( void ) = 0;

	virtual QList<NodeControlInterface::AvailablePinEntry> oscPins( const QStringList &pCurDir ) const = 0;
};

} // namespace osc

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::osc::NamespaceInterface, "com.bigfug.fugio.osc.namespace/1.0" )

#endif // NAMESPACE_INTERFACE_H
