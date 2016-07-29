#include "pinlistmodel.h"

#include "pinmodel.h"

PinListModel::PinListModel( PinDirection pDirection, NodeModel *pParent )
	: mParent( pParent ), mDirection( pDirection )
{

}

int PinListModel::row()
{
	return( mDirection == PIN_INPUT ? 0 : 1 );
}

QVariant PinListModel::data( int ) const
{
	return( mDirection == PIN_INPUT ? "Inputs" : "Outputs" );
}

int PinListModel::rowCount( void ) const
{
	return( mPinList.size() );
}

int PinListModel::columnCount() const
{
	return( 1 );
}

int PinListModel::childRow( PinModel *pChild ) const
{
	return( mPinList.indexOf( pChild ) );
}

PinModel *PinListModel::appendPin( const QUuid &pPinId, QString pName )
{
	PinModel		*Pin = new PinModel( pPinId, mDirection, pName, this );

	if( Pin )
	{
		mPinList << Pin;
	}

	return( Pin );
}

void PinListModel::remPin(PinModel *pPin)
{
	mPinList.removeAll( pPin );
}

PinModel *PinListModel::childAt( int pRow ) const
{
	return( mPinList.at( pRow ) );
}
