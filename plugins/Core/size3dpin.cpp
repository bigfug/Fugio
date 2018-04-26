#include "size3dpin.h"

#include <fugio/core/uuid.h>

Size3dPin::Size3dPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), VariantHelper( QMetaType::QVector3D, PID_SIZE_3D )
{

}
