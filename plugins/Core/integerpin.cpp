#include "integerpin.h"
#include <QSettings>

IntegerPin::IntegerPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), mValue( 0 )
{
}

IntegerPin::~IntegerPin( void )
{
}
