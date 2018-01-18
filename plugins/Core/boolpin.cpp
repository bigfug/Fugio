#include "boolpin.h"

#include <fugio/core/uuid.h>

BoolPin::BoolPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), VariantHelper( QMetaType::Bool, PID_BOOL )
{

}
