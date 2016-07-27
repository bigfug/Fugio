#ifndef PINLISTMODEL_H
#define PINLISTMODEL_H

#include <QUuid>

#include <fugio/pin_interface.h>

#include "basemodel.h"
#include "nodemodel.h"

class PinModel;

class PinListModel : public BaseModel
{
public:
	PinListModel( PinDirection pDirection, NodeModel *pParent = nullptr );

	virtual ~PinListModel( void ) {}

	inline PinDirection direction( void ) const
	{
		return( mDirection );
	}

	inline NodeModel *parent( void ) Q_DECL_OVERRIDE
	{
		return( mParent );
	}

	virtual int row( void ) Q_DECL_OVERRIDE;

	virtual QVariant data( int ) const Q_DECL_OVERRIDE;

	virtual int rowCount( void ) const Q_DECL_OVERRIDE;

	virtual int columnCount() const Q_DECL_OVERRIDE;

	int childRow( PinModel *pChild ) const;

	void appendPinId( const QUuid &pPinId );

	void remPin( PinModel *pPin );

	PinModel *childAt( int pRow ) const;

private:
	NodeModel				*mParent;
	QList<PinModel *>		 mPinList;
	PinDirection			 mDirection;
};

#endif // PINLISTMODEL_H
