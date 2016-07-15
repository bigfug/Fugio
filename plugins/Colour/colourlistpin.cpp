#include "colourlistpin.h"

#include <fugio/colour/colour_interface.h>

ColourListPin::ColourListPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin )
{
}

QUuid ColourListPin::listPinControl() const
{
	return( PID_COLOUR );
}
