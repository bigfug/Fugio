#include "vector4pin.h"

#include <fugio/math/uuid.h>

Vector4Pin::Vector4Pin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), VariantHelper( QMetaType::QVector4D, PID_VECTOR4 )
{

}
