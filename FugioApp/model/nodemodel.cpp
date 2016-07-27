#include "nodemodel.h"

#include "pinmodel.h"
#include "pinlistmodel.h"
#include "baselistmodel.h"

NodeModel::NodeModel( const QUuid &pNodeId, BaseListModel *pParent )
	: mParent( pParent ), mNodeId( pNodeId )
{
	mInputs  = new PinListModel( PIN_INPUT, this );
	mOutputs = new PinListModel( PIN_OUTPUT, this );
	mChildren = new BaseListModel( this );
}

BaseModel *NodeModel::parent( void )
{
	return( mParent );
}

int NodeModel::row()
{
	return( mParent ? mParent->childRow( this ) : 0 );
}

int NodeModel::childRow( BaseModel *pChild )
{
	return( mChildren->childRow( pChild ) );
}

void NodeModel::appendChild( BaseModel *pChild )
{
	mChildren->appendChild( pChild );
}

void NodeModel::removeChild(BaseModel *pChild)
{
	mChildren->removeChild( pChild );
}

void NodeModel::removeChildAt( int pChildIndex )
{
	mChildren->removeChildAt( pChildIndex );
}

QVariant NodeModel::data( int pColumn ) const
{
	return( pColumn == 0 ? mName : QVariant() );
}

