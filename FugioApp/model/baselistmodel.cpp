#include "baselistmodel.h"

BaseListModel::BaseListModel( NodeModel *pParent )
	: mParent( pParent )
{

}

int BaseListModel::row()
{
	return( 2 );
}

QVariant BaseListModel::data(int) const
{
	return( "Children" );
}

int BaseListModel::rowCount() const
{
	return( mBaseList.size() );
}

int BaseListModel::columnCount() const
{
	return( 1 );
}

int BaseListModel::childRow( BaseModel *pChild ) const
{
	return( mBaseList.indexOf( pChild ) );
}

void BaseListModel::appendChild( BaseModel *pChild )
{
	mBaseList << pChild;
}

void BaseListModel::removeChild(BaseModel *pChild)
{
	mBaseList.removeAll( pChild );
}

void BaseListModel::removeChildAt(int pChildIndex)
{
	mBaseList.removeAt( pChildIndex );
}

BaseModel *BaseListModel::childAt( int pRow ) const
{
	return( mBaseList.at( pRow ) );
}
