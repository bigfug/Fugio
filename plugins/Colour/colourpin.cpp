#include "colourpin.h"
#include <QSettings>

ColourPin::ColourPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), VariantHelper( QMetaType::QColor, PID_COLOUR )
{
}

void ColourPin::setFromBaseVariant( int pIndex, int pOffset, const QVariant &pValue )
{
	const int			i = variantIndex( pIndex, pOffset );
	QColor				C = mValues[ i ];

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

	mValues[ i ] = C;
}

QVariant ColourPin::baseVariant( int pIndex, int pOffset ) const
{
	QColor				C = mValues.at( variantIndex( pIndex, pOffset ) );

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

