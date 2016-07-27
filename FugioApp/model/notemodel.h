#ifndef NOTEMODEL_H
#define NOTEMODEL_H

#include <QString>

#include "basemodel.h"
#include "groupmodel.h"

class NoteModel : public BaseModel
{
public:
	NoteModel( GroupModel *pParent = nullptr );

	virtual ~NoteModel( void ) {}

	inline GroupModel *parent( void ) Q_DECL_OVERRIDE
	{
		return( mParent );
	}

	inline void setParent( GroupModel *pParent )
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
	GroupModel			*mParent;
	QString				 mNote;
};

#endif // NOTEMODEL_H
