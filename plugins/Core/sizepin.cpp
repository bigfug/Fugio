#include "sizepin.h"

SizePin::SizePin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), mValues( 1 )
{
}
