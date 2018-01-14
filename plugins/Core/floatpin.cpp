#include "floatpin.h"
#include <QSettings>

FloatPin::FloatPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), mValues( 1 )
{
}

QString FloatPin::toString() const
{
	QStringList		L;

	for( double v : mValues )
	{
		L << QString::number( v );
	}

	return( L.join( ',' ) );
}
