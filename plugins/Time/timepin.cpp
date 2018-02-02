#include "timepin.h"

#include <fugio/time/uuid.h>

TimePin::TimePin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), VariantHelper( QMetaType::QTime, PID_TIME )
{
}

QString TimePin::toString() const
{
	QStringList		L;

	for( QTime v : mValues )
	{
		L << v.toString();
	}

	return( L.join( ',' ) );
}

void TimePin::serialise( QDataStream &pDataStream ) const
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

void TimePin::deserialise( QDataStream &pDataStream )
{
#if QT_VERSION >= QT_VERSION_CHECK( 5, 7, 0 )
	QTime				V;

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

	QVector<QTime>	L;

	pDataStream >> L;

	mValues = L;
}
