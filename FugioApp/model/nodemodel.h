#ifndef NODEMODEL_H
#define NODEMODEL_H

#include <QUuid>
#include <QList>

#include <fugio.h>

#include "basemodel.h"
#include "groupmodel.h"

class PinModel;

class NodeModel : public BaseModel
{
public:
	NodeModel( const QUuid &pNodeId, GroupModel *pParent = nullptr );

	virtual ~NodeModel( void ) {}

	inline GroupModel *parent( void ) Q_DECL_OVERRIDE
	{
		return( mParent );
	}

	virtual int row( void ) Q_DECL_OVERRIDE;

	void setNodeId( const QUuid &pNodeId )
	{
		mNodeId = pNodeId;
	}

	void addPinInput( const QUuid &pPinId );
	void addPinOutput( const QUuid &pPinId );

	void remPin( PinModel *pPin );

	inline int inputCount( void ) const
	{
		return( mInputs.size() );
	}

	inline int outputCount( void ) const
	{
		return( mOutputs.size() );
	}

	inline int inputRow( PinModel *pChild ) const
	{
		return( mInputs.indexOf( pChild ) );
	}

	inline int outputRow( PinModel *pChild ) const
	{
		return( mOutputs.indexOf( pChild ) );
	}

	virtual bool isGroup( void ) const Q_DECL_OVERRIDE
	{
		return( false );
	}

	inline QUuid id( void ) const
	{
		return( mNodeId );
	}

	virtual int rowCount( int pColumn ) const Q_DECL_OVERRIDE
	{
		if( pColumn == 0 )
		{
			return( mInputs.size() );
		}

		if( pColumn == 1 )
		{
			return( mOutputs.size() );
		}

		return( 0 );
	}

	virtual int columnCount( void ) const Q_DECL_OVERRIDE
	{
		return( 2 );
	}

	virtual QVariant data( int pColumn ) const Q_DECL_OVERRIDE
	{
		return( pColumn == 0 ? mNodeId.toString() : QVariant() );
	}

private:
	GroupModel				*mParent;
	QUuid					 mNodeId;
	QList<PinModel *>		 mInputs;
	QList<PinModel *>		 mOutputs;
};

#endif // NODEMODEL_H
