#include "transformpin.h"

#include <fugio/core/uuid.h>

TransformPin::TransformPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), VariantHelper( QMetaType::QTransform, PID_TRANSFORM )
{
}
