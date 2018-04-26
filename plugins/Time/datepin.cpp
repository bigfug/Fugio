#include "datepin.h"

#include <fugio/time/uuid.h>

DatePin::DatePin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), VariantHelper( QMetaType::QDate, PID_DATE )
{
}

QString DatePin::toString() const
{
	QStringList		L;

	for( QDate v : mValues )
	{
		L << v.toString();
	}

	return( L.join( ',' ) );
}

void DatePin::serialise( QDataStream &pDataStream ) const
{
	if( mValues.size() == 1 )
	{
		pDataStream << mValues.first();
	}
	else
	{
		pDataStream << mValues;
	}
}

void DatePin::deserialise( QDataStream &pDataStream )
{
#if QT_VERSION >= QT_VERSION_CHECK( 5, 7, 0 )
	QDate				V;

	pDataStream.startTransaction();

	pDataStream >> V;

	if( pDataStream.commitTransaction() )
	{
		mValues.resize( 1 );

		setVariant( 0, V );

		return;
	}

	pDataStream.rollbackTransaction();
#endif

	QVector<QDate>	L;

	pDataStream >> L;

	mValues = L;
}
