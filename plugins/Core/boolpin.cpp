#include "boolpin.h"

#include <fugio/core/uuid.h>

BoolPin::BoolPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), VariantHelper( QMetaType::Bool, PID_BOOL )
{

}

QString BoolPin::toString() const
{
	QStringList		L;

	for( bool v : mValues )
	{
		L << ( v ? "1" : "0" );
	}

	return( L.join( ',' ) );
}
