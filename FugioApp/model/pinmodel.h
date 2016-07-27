#ifndef PINMODEL_H
#define PINMODEL_H

#include <QUuid>

#include <fugio/pin_interface.h>

#include "basemodel.h"
#include "pinlistmodel.h"

class PinModel : public BaseModel
{
public:
	PinModel( const QUuid &pPinId, PinDirection pDirection, PinListModel *pParent = nullptr );

	virtual ~PinModel( void ) {}

	inline PinDirection direction( void ) const
	{
		return( mDirection );
	}

	inline PinListModel *parent( void ) Q_DECL_OVERRIDE
	{
		return( mParent );
	}

	virtual int row( void ) Q_DECL_OVERRIDE;

	inline void setId( const QUuid &pPinId )
	{
		mPinGlobalId = pPinId;
	}

	inline QUuid globalId( void ) const
	{
		return( mPinGlobalId );
	}

	virtual QVariant data( int pColumn ) const Q_DECL_OVERRIDE
	{
		return( pColumn == 0 ? mPinGlobalId.toString() : QVariant() );
	}

private:
	PinListModel	*mParent;
	QUuid			 mPinGlobalId;
	PinDirection	 mDirection;
};

#endif // PINMODEL_H
