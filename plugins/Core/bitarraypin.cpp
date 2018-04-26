#include "bitarraypin.h"

#include <fugio/core/uuid.h>

BitArrayPin::BitArrayPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), VariantHelper( QMetaType::QBitArray, PID_BITARRAY )
{
}

