#include "colourpin.h"
#include <QSettings>

ColourPin::ColourPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), mColours( 1 )
{
}

void ColourPin::setFromBaseVariant(int pIndex, const QVariant &pValue)
{
	QColor				C = mColours[ pIndex ];

	if( pValue.type() == QVariant::Map )
	{
		QVariantMap			M = pValue.toMap();

		C.setRedF( M.value( "r", C.redF() ).toReal() );
		C.setGreenF( M.value( "g", C.greenF() ).toReal() );
		C.setBlueF( M.value( "b", C.blueF() ).toReal() );
		C.setAlphaF( M.value( "a", C.alphaF() ).toReal() );
	}
	else if( pValue.type() == QVariant::List )
	{
		QList<QVariant>     L = pValue.toList();

		if( L.size() > 0 ) C.setRedF( L.at( 0 ).toReal() );
		if( L.size() > 1 ) C.setGreenF( L.at( 1 ).toReal() );
		if( L.size() > 2 ) C.setBlueF( L.at( 2 ).toReal() );
		if( L.size() > 3 ) C.setAlphaF( L.at( 3 ).toReal() );
	}

	mColours[ pIndex ] = C;
}

QVariant ColourPin::baseVariant( int pIndex ) const
{
	QColor				C = mColours.at( pIndex );

//	QList<QVariant>		L;

//	L << C.redF();
//	L << C.greenF();
//	L << C.blueF();
//	L << C.alphaF();

//	return( L );

	QVariantMap			M;

	M.insert( "r", C.redF() );
	M.insert( "g", C.greenF() );
	M.insert( "b", C.blueF() );
	M.insert( "a", C.alphaF() );

	return( M );
}

