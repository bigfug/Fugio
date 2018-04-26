#include "floatpin.h"
#include <fugio/core/uuid.h>

FloatPin::FloatPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), VariantHelper( QMetaType::Double, PID_FLOAT )
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
