#include "nodemodel.h"

#include "pinmodel.h"
#include "pinlistmodel.h"

NodeModel::NodeModel( const QUuid &pNodeId, GroupModel *pParent )
	: mParent( pParent ), mNodeId( pNodeId )
{
	mInputs  = new PinListModel( PIN_INPUT, this );
	mOutputs = new PinListModel( PIN_OUTPUT, this );
}

int NodeModel::row()
{
	return( mParent ? mParent->childRow( this ) : 0 );
}

