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

	inline void setParent( GroupModel *pParent )
	{
		mParent = pParent;
	}

	virtual int row( void ) Q_DECL_OVERRIDE;

	void appendChild( BaseModel *pChild );

	void removeChild( int pRow );

	BaseModel *childAt( int pRow ) const;

	inline int childRow( BaseModel *pChild ) const
	{
		return( mChildren.indexOf( pChild ) );
	}

	virtual int rowCount( void ) const Q_DECL_OVERRIDE
	{
		return( mChildren.size() );
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
