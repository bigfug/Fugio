#ifndef NOTEMODEL_H
#define NOTEMODEL_H

#include <QString>

#include "basemodel.h"
#include "nodemodel.h"

class NoteModel : public BaseModel
{
public:
	NoteModel( BaseListModel *pParent = nullptr );

	virtual ~NoteModel( void ) {}

	BaseModel *parent( void ) Q_DECL_OVERRIDE;

	BaseListModel *parentList( void )
	{
		return( mParent );
	}

	inline void setParent( BaseListModel *pParent )
	{
		mParent = pParent;
	}

	inline QString note( void ) const
	{
		return( mNote );
	}

	virtual QVariant data( int pColumn ) const Q_DECL_OVERRIDE
	{
		return( pColumn == 0 ? mNote : QVariant() );
	}

	virtual int row( void ) Q_DECL_OVERRIDE;

private:
	BaseListModel		*mParent;
	QString				 mNote;
};

#endif // NOTEMODEL_H
