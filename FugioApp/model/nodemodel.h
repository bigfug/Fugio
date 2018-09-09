#ifndef NODEMODEL_H
#define NODEMODEL_H

#include <QUuid>
#include <QList>

#include <fugio.h>

#include "basemodel.h"

class PinModel;
class PinListModel;
class BaseListModel;

class NodeModel : public BaseModel
{
public:
	NodeModel( const QUuid &pNodeId, BaseListModel *pParent = nullptr );

	virtual ~NodeModel( void ) Q_DECL_OVERRIDE {}

	virtual BaseModel *parent( void ) Q_DECL_OVERRIDE;

	BaseListModel *parentList( void )
	{
		return( mParent );
	}

	inline void setParent( BaseListModel *pParent )
	{
		mParent = pParent;
	}

	inline void setName( QString pName )
	{
		mName = pName;
	}

	virtual int row( void ) Q_DECL_OVERRIDE;

	int childRow( BaseModel *pChild );

	void appendChild( BaseModel *pChild );

	void removeChild( BaseModel *pChild );

	void removeChildAt( int pChildIndex );

	void setNodeId( const QUuid &pNodeId )
	{
		mNodeId = pNodeId;
	}

	PinListModel *inputs( void )
	{
		return( mInputs );
	}

	PinListModel *outputs( void )
	{
		return( mOutputs );
	}

	BaseListModel *children( void )
	{
		return( mChildren );
	}

	inline QUuid id( void ) const
	{
		return( mNodeId );
	}

	virtual int rowCount( void ) const Q_DECL_OVERRIDE
	{
		return( 3 );
	}

	virtual int columnCount( void ) const Q_DECL_OVERRIDE
	{
		return( 1 );
	}

	virtual QVariant data( int pColumn ) const Q_DECL_OVERRIDE;

private:
	BaseListModel			*mParent;
	QString					 mName;
	QUuid					 mNodeId;
	PinListModel			*mInputs;
	PinListModel			*mOutputs;
	BaseListModel			*mChildren;
};

#endif // NODEMODEL_H
