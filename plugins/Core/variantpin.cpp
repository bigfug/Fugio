#include "variantpin.h"

#include <QPointF>
#include <QSizeF>

#include <fugio/core/uuid.h>

VariantPin::VariantPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), VariantHelper( QMetaType::UnknownType, PID_VARIANT )
{

}

QString VariantPin::toString() const
{
	QStringList		L;

	for( QVariant v : mValues )
	{
		switch( QMetaType::Type( v.type() ) )
		{
			case QMetaType::QPointF:
				{
					QPointF		V = v.value<QPointF>();

					L << QString( "%1,%2 (%3)" ).arg( V.x() ).arg( V.y() ).arg( QString( v.typeName() ) );
				}

			case QMetaType::QSizeF:
				{
					QSizeF		V = v.value<QSizeF>();

					L << QString( "%1,%2 (%3)" ).arg( V.width() ).arg( V.height() ).arg( QString( v.typeName() ) );
				}

			default:
				L << v.toString();
				break;
		}
	}

	return( L.join( ',' ) );
}

