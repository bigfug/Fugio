#include "fontpin.h"

#include <fugio/painter/uuid.h>

FontPin::FontPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), VariantHelper( QMetaType::QFont, PID_FONT )
{
}

QString FontPin::toString() const
{
	QStringList		L;

	for( QFont v : mValues )
	{
		L << v.toString();
	}

	return( L.join( ',' ) );
}

void FontPin::serialise( QDataStream &pDataStream ) const
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

void FontPin::deserialise( QDataStream &pDataStream )
{
#if QT_VERSION >= QT_VERSION_CHECK( 5, 7, 0 )
	QFont				V;

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

	QVector<QFont>	L;

	pDataStream >> L;

	mValues = L;
}
