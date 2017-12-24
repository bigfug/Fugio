#include "listpin.h"

ListPin::ListPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), mValues( 1 )
{

}
