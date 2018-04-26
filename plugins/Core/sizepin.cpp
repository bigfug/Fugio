#include "sizepin.h"

#include <fugio/core/uuid.h>

SizePin::SizePin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), VariantHelper( QMetaType::QSizeF, PID_SIZE )
{
}
