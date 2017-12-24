#include "pointpin.h"

PointPin::PointPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), mValues( 1 )
{

}
