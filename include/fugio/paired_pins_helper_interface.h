#ifndef PAIRED_PINS_HELPER_INTERFACE_H
#define PAIRED_PINS_HELPER_INTERFACE_H

#include <QSharedPointer>

#include <QUuid>

#include <fugio/global.h>

FUGIO_NAMESPACE_BEGIN

class PinInterface;

class PairedPinsHelperInterface
{
public:
	virtual ~PairedPinsHelperInterface( void ) {}

	virtual QUuid pairedPinControlUuid( QSharedPointer<fugio::PinInterface> pPin ) const = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::PairedPinsHelperInterface, "com.bigfug.fugio.helper.paired-pins/1.0" )

#endif // PAIRED_PINS_HELPER_INTERFACE_H
