#include "quaternionpin.h"

#include <QSettings>

#include <fugio/core/uuid.h>

QuaternionPin::QuaternionPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin )
{
}

QuaternionPin::~QuaternionPin()
{

}

QString QuaternionPin::toString() const
{
	return( QString( "w: %1<br/>x: %2<br/>y: %3<br/>z: %4<br/>" ).arg( mValue.scalar() ).arg( mValue.x() ).arg( mValue.y() ).arg( mValue.z() ) );
}

void QuaternionPin::loadSettings( QSettings &pSettings )
{
	mValue = pSettings.value( "quaternion", mValue ).value<QQuaternion>();
}

void QuaternionPin::saveSettings( QSettings &pSettings ) const
{
	pSettings.setValue( "quaternion", mValue );
}


int QuaternionPin::listSize() const
{
	return( 4 );
}

QUuid QuaternionPin::listPinControl() const
{
	return( PID_FLOAT );
}

QVariant QuaternionPin::listIndex(int pIndex) const
{
	if( pIndex == 0 ) return( mValue.scalar() );
	if( pIndex == 1 ) return( mValue.x() );
	if( pIndex == 2 ) return( mValue.y() );
	if( pIndex == 3 ) return( mValue.z() );

	return( QVariant() );
}

void QuaternionPin::listSetIndex(int pIndex, const QVariant &pValue)
{
	if( pIndex == 0 ) mValue.setScalar( pValue.toFloat() );
	if( pIndex == 1 ) mValue.setX( pValue.toFloat() );
	if( pIndex == 2 ) mValue.setY( pValue.toFloat() );
	if( pIndex == 3 ) mValue.setZ( pValue.toFloat() );
}

void QuaternionPin::listSetSize(int pSize)
{
	Q_UNUSED( pSize )
}

void QuaternionPin::listClear()
{
}

void QuaternionPin::listAppend(const QVariant &pValue)
{
	Q_UNUSED( pValue );
}

bool QuaternionPin::listIsEmpty() const
{
	return( false );
}
