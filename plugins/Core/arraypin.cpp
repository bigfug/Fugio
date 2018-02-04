#include "arraypin.h"

#include <fugio/core/uuid.h>

#include <QRect>
#include <QRectF>
#include <QLineF>
#include <QColor>

#include "coreplugin.h"

ArrayPin::ArrayPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), mData( nullptr ), mType( QMetaType::UnknownType ), mStride( 0 ),
	  mCount( 0 ), mSize( 0 ), mReserve( 0 )
{

}

QString ArrayPin::toString() const
{
	return( QString( "<p>Type: %1</p><p>Count: %2</p><p>Size: %3</p><p>Total Size: %4</p>" ).arg( QString( QMetaType::typeName( type() ) ) ).arg( count() ).arg( size() ).arg( count() * stride() ) );
}
