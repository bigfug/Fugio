#include "linepin.h"

#include <fugio/core/uuid.h>

LinePin::LinePin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), VariantHelper( QMetaType::QLineF, PID_LINE )
{

}
