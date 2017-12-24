#include "rectpin.h"

RectPin::RectPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), mValues( 1 )
{

}
