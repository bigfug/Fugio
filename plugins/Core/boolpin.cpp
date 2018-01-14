#include "boolpin.h"

BoolPin::BoolPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), mValues( 1 )
{

}
