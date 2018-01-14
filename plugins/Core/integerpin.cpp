#include "integerpin.h"
#include <QSettings>

IntegerPin::IntegerPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), mValues( 1 )
{
}

IntegerPin::~IntegerPin( void )
{
}
