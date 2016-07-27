#include "groupmodel.h"

GroupModel::GroupModel( GroupModel *pParent )
	: mParent( pParent )
{

}

GroupModel::~GroupModel()
{
	qDeleteAll( mChildren );
}

int GroupModel::row()
{
	return( mParent ? mParent->childRow( this ) : 0 );
}

void GroupModel::appendChild( BaseModel *pChild )
{
	mChildren << pChild;
}

void GroupModel::removeChild( int pRow )
{
	mChildren.removeAt( pRow );
}

BaseModel *GroupModel::childAt( int pRow ) const
{
	return( mChildren.at( pRow ) );
}
