#include "listpin.h"

ListPin::ListPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), VariantHelper( QMetaType::QVariantList, PID_VARIANT_LIST )
{

}
