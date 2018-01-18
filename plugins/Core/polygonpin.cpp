#include "polygonpin.h"

#include <fugio/core/uuid.h>

PolygonPin::PolygonPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), VariantHelper( QMetaType::QPolygonF, PID_POLYGON )
{

}
