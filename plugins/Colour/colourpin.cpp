#include "colourpin.h"
#include <QSettings>

ColourPin::ColourPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin )
{
}

ColourPin::~ColourPin()
{
}
