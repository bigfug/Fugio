#ifndef BASELISTMODEL_H
#define BASELISTMODEL_H

#include <QUuid>

#include <fugio/pin_interface.h>

#include "basemodel.h"
#include "nodemodel.h"

class BaseListModel : public BaseModel
{
public:
	BaseListModel( NodeModel *pParent = nullptr );

	virtual ~BaseListModel( void ) Q_DECL_OVERRIDE {}

	inline NodeModel *parent( void ) Q_DECL_OVERRIDE
	{
		return( mParent );
	}

	virtual int row( void ) Q_DECL_OVERRIDE;

	virtual QVariant data( int ) const Q_DECL_OVERRIDE;

	virtual int rowCount( void ) const Q_DECL_OVERRIDE;

	virtual int columnCount() const Q_DECL_OVERRIDE;

	int childRow( BaseModel *pChild ) const;

	void appendChild( BaseModel *pChild );

	void removeChild( BaseModel *pChild );

	void removeChildAt( int pChildIndex );

	BaseModel *childAt( int pRow ) const;

private:
	NodeModel				*mParent;
	QList<BaseModel *>		 mBaseList;
};

#endif // BASELISTMODEL_H
