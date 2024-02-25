#ifndef SERIALISE_H
#define SERIALISE_H

#include <QDataStream>

#include "global.h"

FUGIO_NAMESPACE_BEGIN

class SerialiseInterface
{
public:
	virtual ~SerialiseInterface( void ) {}

	virtual void serialise( QDataStream &pDataStream ) const = 0;

	virtual void deserialise( QDataStream &pDataStream ) = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::SerialiseInterface, "com.bigfug.fugio.serialise/1.0" )

#endif // SERIALISE_H
