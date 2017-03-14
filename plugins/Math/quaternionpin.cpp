#include "quaternionpin.h"

#include <QSettings>

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
