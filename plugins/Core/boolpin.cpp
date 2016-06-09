#include "boolpin.h"

BoolPin::BoolPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), mValue( false )
{

}
