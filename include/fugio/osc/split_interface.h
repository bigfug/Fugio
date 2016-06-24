#ifndef FUGIO_OSC_SPLIT_INTERFACE_H
#define FUGIO_OSC_SPLIT_INTERFACE_H

#include <QtPlugin>

#include <fugio/global.h>

FUGIO_NAMESPACE_BEGIN

namespace osc {

class SplitInterface
{
public:
	virtual ~SplitInterface( void ) {}

	virtual void oscSplit( QStringList pPath, const QVariant &pValue ) = 0;

	virtual void oscPath( QStringList &pPath ) const = 0;
};

} // namespace osc

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::osc::SplitInterface, "com.bigfug.fugio.osc.split/1.0" )

#endif // FUGIO_OSC_SPLIT_INTERFACE_H
