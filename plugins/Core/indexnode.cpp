#include "indexnode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>

IndexNode::IndexNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mLastUpdate( 0 )
{
	FUGID( PIN_OUTPUT_INDEX,	"9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_OUTPUT_VALUE,	"1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_INPUT_1,			"261cc653-d7fa-4c34-a08b-3603e8ae71d5" );

	mOutputIndex = pinOutput<fugio::VariantInterface *>( "Index", mPinOutputIndex, PID_INTEGER, PIN_OUTPUT_INDEX );

	mNode->createPin( "Value", PIN_OUTPUT, PIN_OUTPUT_VALUE, mPinOutputValue, QUuid() );

	QSharedPointer<fugio::PinInterface>	PinI = pinInput( "Input", PIN_INPUT_1 );

	PinI->setAutoRename( true );
}

void IndexNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	QList< QSharedPointer<fugio::PinInterface> >	PinLst = mNode->enumInputPins();

	QSharedPointer<fugio::PinInterface>			UpdatedPin;
	int											UpdatedIdx = 0;

	for( int i = 0 ; i < PinLst.size() ; i++ )
	{
		QSharedPointer<fugio::PinInterface>	P = PinLst.at( i );

		if( P->updated() <= mLastUpdate )
		{
			continue;
		}

		UpdatedPin = P;
		UpdatedIdx = i;

		mLastUpdate = P->updated();
	}

	if( UpdatedPin )
	{
		if( mOutputIndex->variant().toInt() != UpdatedIdx )
		{
			mOutputIndex->setVariant( UpdatedIdx );

			pinUpdated( mPinOutputIndex );
		}

		if( UpdatedPin->isConnected() && UpdatedPin->connectedPin()->hasControl() )
		{
			mPinOutputValue->setControl( UpdatedPin->connectedPin()->control() );
		}
		else
		{
			mPinOutputValue->setControl( QSharedPointer<fugio::PinControlInterface>() );
		}

		pinUpdated( mPinOutputValue );
	}
}

QList<QUuid> IndexNode::pinAddTypesInput() const
{
	return( QList<QUuid>() );
}

bool IndexNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_OUTPUT );
}

bool IndexNode::pinShouldAutoRename( fugio::PinInterface *pPin ) const
{
	Q_UNUSED( pPin )

	return( true );
}
