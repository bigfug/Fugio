#include "bytearraylistpin.h"

#include <fugio/core/uuid.h>

ByteArrayListPin::ByteArrayListPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin )
{
}

QUuid ByteArrayListPin::listPinControl() const
{
	return( PID_BYTEARRAY );
}

void ByteArrayListPin::listSetIndex(int pIndex, const QVariant &pValue)
{

}
