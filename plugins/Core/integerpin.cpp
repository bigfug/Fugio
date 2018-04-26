#include "integerpin.h"
#include <fugio/core/uuid.h>

IntegerPin::IntegerPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), VariantHelper( QMetaType::Int, PID_INTEGER )
{
}
