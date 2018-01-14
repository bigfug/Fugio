#include "stringpin.h"
#include <QSettings>

StringPin::StringPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), mValues( 1 )
{
}
