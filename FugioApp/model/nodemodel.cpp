#include "nodemodel.h"

#include "pinmodel.h"

NodeModel::NodeModel( const QUuid &pNodeId, GroupModel *pParent )
	: mParent( pParent ), mNodeId( pNodeId )
{

}

int NodeModel::row()
{
	return( mParent ? mParent->childRow( this ) : 0 );
}

void NodeModel::addPinInput( const QUuid &pPinId )
{
	mInputs << new PinModel( pPinId, PIN_INPUT, this );
}

void NodeModel::addPinOutput( const QUuid &pPinId )
{
	mOutputs << new PinModel( pPinId, PIN_OUTPUT, this );
}

void NodeModel::remPin( PinModel *pPin )
{
	if( pPin->direction() == PIN_INPUT )
	{
		mInputs.removeAll( pPin );
	}
	else
	{
		mOutputs.removeAll( pPin );
	}
}
