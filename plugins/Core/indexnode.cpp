#include "indexnode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>

IndexNode::IndexNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mOutputIndex = pinOutput<fugio::VariantInterface *>( "Index", mPinOutputIndex, PID_INTEGER );

	mNode->createPin( "Value", PIN_OUTPUT, next_uuid(), mPinOutputValue, QUuid() );

	pinInput( "Input 1" );
}

void IndexNode::inputsUpdated( qint64 pTimeStamp )
{
	QList< QSharedPointer<fugio::PinInterface> >	PinLst = mNode->enumInputPins();

	for( int i = 0 ; i < PinLst.size() ; i++ )
	{
		QSharedPointer<fugio::PinInterface>	P = PinLst.at( i );

		if( !P->isUpdated( pTimeStamp ) )
		{
			continue;
		}

		if( !pTimeStamp || mOutputIndex->variant().toInt() != i )
		{
			mOutputIndex->setVariant( i );

			pinUpdated( mPinOutputIndex );

			if( P->isConnected() && P->connectedPin()->hasControl() )
			{
				mPinOutputValue->setControl( P->connectedPin()->control() );
			}
			else
			{
				mPinOutputValue->setControl( QSharedPointer<fugio::PinControlInterface>() );
			}

			pinUpdated( mPinOutputValue );
		}

		break;
	}
}

QList<QUuid> IndexNode::pinAddTypesInput() const
{
	return( mNode->context()->global()->pinIds().keys() );
}

bool IndexNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_OUTPUT );
}
