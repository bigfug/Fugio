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
		L << v.toString();
	}

	return( L.join( ',' ) );
//	switch( QMetaType::Type( mValue.type() ) )
//	{
//		case QMetaType::QPointF:
//			{
//				QPointF		V = mValue.value<QPointF>();

//				return( QString( "%1,%2 (%3)" ).arg( V.x() ).arg( V.y() ) ).arg( QString( mValue.typeName() ) );
//			}

//		case QMetaType::QSizeF:
//			{
//				QSizeF		V = mValue.value<QSizeF>();

//				return( QString( "%1,%2 (%3)" ).arg( V.width() ).arg( V.height() ) ).arg( QString( mValue.typeName() ) );
//			}

//		default:
//			break;
//	}

//	return( QString( "%1 (%2)" ).arg( mValue.toString() ).arg( QString( mValue.typeName() ) ) );

//	return( QString() );
}

