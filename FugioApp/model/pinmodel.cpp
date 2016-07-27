#include "pinmodel.h"

#include "pinlistmodel.h"

PinModel::PinModel( const QUuid &pPinId, PinDirection PinDirection, PinListModel *pParent )
	: mParent( pParent ), mPinGlobalId( pPinId ), mDirection( PinDirection )
{

}

int PinModel::row()
{
	return( mParent ? mParent->childRow( this ) : 0 );
}
