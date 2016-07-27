#ifndef GROUPMODEL_H
#define GROUPMODEL_H

#include <QList>

#include "basemodel.h"

class GroupModel : public BaseModel
{
public:
	GroupModel( GroupModel *pParent = nullptr );

	virtual ~GroupModel( void );

	inline GroupModel *parent( void ) Q_DECL_OVERRIDE
	{
		return( mParent );
	}

	virtual int row( void ) Q_DECL_OVERRIDE;

	void appendChild( BaseModel *pChild );

	void removeChild( int pRow );

	BaseModel *childAt( int pRow ) const;

	inline int childRow( BaseModel *pChild ) const
	{
		return( mChildren.indexOf( pChild ) );
	}

	virtual bool isGroup( void ) const Q_DECL_OVERRIDE
	{
		return( true );
	}

	virtual int rowCount( int pColumn ) const Q_DECL_OVERRIDE
	{
		return( pColumn <= 0 ? mChildren.size() : 0 );
	}

	virtual int columnCount( void ) const Q_DECL_OVERRIDE
	{
		return( 1 );
	}

	virtual QVariant data( int pColumn ) const Q_DECL_OVERRIDE
	{
		return( pColumn == 0 ? "Group" : QVariant() );
	}

private:
	GroupModel				*mParent;
	QList<BaseModel *>		 mChildren;
};

#endif // GROUPMODEL_H
