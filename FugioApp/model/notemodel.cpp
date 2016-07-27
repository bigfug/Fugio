#include "notemodel.h"

NoteModel::NoteModel( GroupModel *pParent )
	: mParent( pParent )
{

}

int NoteModel::row()
{
	return( mParent ? mParent->childRow( this ) : 0 );
}
