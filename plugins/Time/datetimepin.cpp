#include "datetimepin.h"

#include <fugio/time/uuid.h>

DateTimePin::DateTimePin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), VariantHelper( QMetaType::QDateTime, PID_DATETIME )
{
}

QString DateTimePin::toString() const
{
	QStringList		L;

	for( QDateTime v : mValues )
	{
		L << v.toString();
	}

	return( L.join( ',' ) );
}

void DateTimePin::serialise( QDataStream &pDataStream ) const
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

void DateTimePin::deserialise( QDataStream &pDataStream )
{
#if QT_VERSION >= QT_VERSION_CHECK( 5, 7, 0 )
	QDateTime				V;

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

	QVector<QDateTime>	L;

	pDataStream >> L;

	mValues = L;
}
