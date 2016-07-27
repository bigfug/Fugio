#ifndef NODEMODEL_H
#define NODEMODEL_H

#include <QUuid>
#include <QList>

#include <fugio.h>

#include "basemodel.h"
#include "groupmodel.h"

class PinModel;
class PinListModel;

class NodeModel : public BaseModel
{
public:
	NodeModel( const QUuid &pNodeId, GroupModel *pParent = nullptr );

	virtual ~NodeModel( void ) {}

	inline GroupModel *parent( void ) Q_DECL_OVERRIDE
	{
		return( mParent );
	}

	inline void setParent( GroupModel *pParent )
	{
		mParent = pParent;
	}

	virtual int row( void ) Q_DECL_OVERRIDE;

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

	inline QUuid id( void ) const
	{
		return( mNodeId );
	}

	virtual int rowCount( void ) const Q_DECL_OVERRIDE
	{
		return( 2 );
	}

	virtual int columnCount( void ) const Q_DECL_OVERRIDE
	{
		return( 1 );
	}

	virtual QVariant data( int pColumn ) const Q_DECL_OVERRIDE
	{
		return( pColumn == 0 ? mNodeId.toString() : QVariant() );
	}

private:
	GroupModel				*mParent;
	QUuid					 mNodeId;
	PinListModel			*mInputs;
	PinListModel			*mOutputs;
};

#endif // NODEMODEL_H
