#include "floatpin.h"
#include <QSettings>

FloatPin::FloatPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), mValue( 0.0 )
{
}

FloatPin::~FloatPin( void )
{
}
