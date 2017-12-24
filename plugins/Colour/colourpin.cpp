#include "colourpin.h"
#include <QSettings>

ColourPin::ColourPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), mColours( 1 )
{
}

ColourPin::~ColourPin()
{
}
