#include "matrix4pin.h"

#include <QSettings>

#include <fugio/math/uuid.h>

Matrix4Pin::Matrix4Pin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), VariantHelper( QMetaType::QMatrix4x4, PID_MATRIX4 )
{
}

QString Matrix4Pin::toString() const
{
	QString		S;

//	for( int i = 0 ; i < 4 ; i++ )
//	{
//		QStringList	SL;

//		QVector4D	r = mValue.row( i );

//		for( int j = 0 ; j < 4 ; j++ )
//		{
//			SL.append( QString( "%1" ).arg( r[ j ] ) );
//		}

//		S.append( "[" + SL.join( "," ) + "]<br/>" );
//	}

	return( S );
}

void Matrix4Pin::loadSettings( QSettings &pSettings )
{
//	mValue = pSettings.value( "matrix", mValue ).value<QMatrix4x4>();
}

void Matrix4Pin::saveSettings(QSettings &pSettings) const
{
//	pSettings.setValue( "matrix", mValue );
}
