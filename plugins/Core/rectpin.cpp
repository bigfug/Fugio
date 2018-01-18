#include "rectpin.h"

#include <fugio/core/uuid.h>

RectPin::RectPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), VariantHelper( QMetaType::QRectF, PID_RECT )
{

}

QString RectPin::toString() const
{
	QStringList		SL;

	for( const QRectF &R : mValues )
	{
		SL << QString( "x=%1 y=%2 w=%3 h=%4" ).arg( R.x() ).arg( R.y() ).arg( R.width() ).arg( R.height() );
	}

	return( SL.join( ", " ) );
}
