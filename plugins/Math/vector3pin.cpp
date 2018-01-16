#include "vector3pin.h"

#include <fugio/math/uuid.h>

Vector3Pin::Vector3Pin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), VariantHelper( QMetaType::QVector3D, PID_VECTOR3 )
{

}
