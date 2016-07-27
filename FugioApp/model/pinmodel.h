#ifndef PINMODEL_H
#define PINMODEL_H

#include <QUuid>

#include <fugio/pin_interface.h>

#include "basemodel.h"
#include "nodemodel.h"

class PinModel : public BaseModel
{
public:
	PinModel( const QUuid &pPinId, PinDirection pDirection, NodeModel *pParent = nullptr );

	virtual ~PinModel( void ) {}

	inline PinDirection direction( void ) const
	{
		return( mDirection );
	}

	inline NodeModel *parent( void ) Q_DECL_OVERRIDE
	{
		return( mParent );
	}

	virtual int row( void ) Q_DECL_OVERRIDE;

	inline void setId( const QUuid &pPinId )
	{
		mPinGlobalId = pPinId;
	}

	virtual bool isGroup( void ) const Q_DECL_OVERRIDE
	{
		return( false );
	}

	inline QUuid globalId( void ) const
	{
		return( mPinGlobalId );
	}

private:
	NodeModel		*mParent;
	QUuid			 mPinGlobalId;
	PinDirection	 mDirection;
};

#endif // PINMODEL_H
