#include "stringlistpin.h"

#include <fugio/core/uuid.h>

StringListPin::StringListPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin )
{
}

QUuid StringListPin::listPinControl() const
{
	return( PID_STRING );
}

void StringListPin::listSetIndex( int pIndex, const QVariant &pValue )
{

}
