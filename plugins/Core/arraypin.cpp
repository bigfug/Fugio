#include "arraypin.h"

ArrayPin::ArrayPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), mData( nullptr ), mType( QMetaType::UnknownType ), mStride( 0 ), mCount( 0 ), mSize( 0 )
{

}

QString ArrayPin::toString() const
{
	return( QString( "<p>Count: %1</p><p>Size: %2</p><p>Total Size: %2</p>" ).arg( count() ).arg( size() ).arg( count() * stride() ) );
}

void ArrayPin::loadSettings( QSettings &pSettings )
{
	mArray = pSettings.value( "array", mArray ).toByteArray();
}

void ArrayPin::saveSettings( QSettings &pSettings ) const
{
	pSettings.setValue( "array", mArray );
}
