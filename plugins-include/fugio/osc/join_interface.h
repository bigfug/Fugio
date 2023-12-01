#ifndef FUGIO_OSC_JOIN_INTERFACE_H
#define FUGIO_OSC_JOIN_INTERFACE_H

#include <QtPlugin>

#include <fugio/global.h>

FUGIO_NAMESPACE_BEGIN

namespace osc {

class JoinInterface
{
public:
	virtual ~JoinInterface( void ) {}

	virtual void oscJoin( QStringList pPath, const QVariant &pValue ) = 0;

	virtual void oscPath( QStringList &pPath ) const = 0;
};

} // namespace osc

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::osc::JoinInterface, "com.bigfug.fugio.osc.join/1.0" )

#endif // FUGIO_OSC_JOIN_INTERFACE_H
