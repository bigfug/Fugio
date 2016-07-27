#include "notemodel.h"

#include "baselistmodel.h"

NoteModel::NoteModel( BaseListModel *pParent )
	: mParent( pParent )
{
	mNote = "Note";
}

BaseModel *NoteModel::parent( void )
{
	return( mParent );
}

int NoteModel::row()
{
	return( mParent ? mParent->childRow( this ) : 0 );
}
