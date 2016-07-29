#include "pinmodel.h"

#include "pinlistmodel.h"

PinModel::PinModel( const QUuid &pPinId, PinDirection PinDirection, QString pName, PinListModel *pParent )
	: mParent( pParent ), mPinGlobalId( pPinId ), mDirection( PinDirection ), mName( pName )
{

}

int PinModel::row()
{
	return( mParent ? mParent->childRow( this ) : 0 );
}
