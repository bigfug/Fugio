#include "pinmodel.h"

#include "nodemodel.h"

PinModel::PinModel( const QUuid &pPinId, PinDirection PinDirection, NodeModel *pParent )
	: mParent( pParent ), mPinGlobalId( pPinId ), mDirection( PinDirection )
{

}

int PinModel::row()
{
	if( mParent )
	{
		if( mDirection == PIN_INPUT )
		{
			return( mParent->inputRow( this ) );
		}
		else
		{
			return( mParent->outputRow( this ) );
		}
	}

	return( 0 );
}
