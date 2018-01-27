#include "pointpin.h"

#include <fugio/core/uuid.h>

PointPin::PointPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), VariantHelper( QMetaType::QPointF, PID_POINT )
{

}
