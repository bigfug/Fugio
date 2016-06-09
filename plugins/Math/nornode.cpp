#include "nornode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>

NorNode::NorNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	static const QUuid	PII_NUMBER1( "{c13a41c6-544b-46bb-a9f2-19dd156d236c}" );
	static const QUuid	PII_NUMBER2( "{608ac771-490b-4ae6-9c81-12b9af526d09}" );

	pinInput( "Boolean", PII_NUMBER1 );
	pinInput( "Boolean", PII_NUMBER2 );

	mValOutput = pinOutput<fugio::VariantInterface *>( "Boolean", mPinOutput, PID_BOOL );
}

void NorNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	bool		OutVal;
	bool		OutHas = false;

	for( QSharedPointer<fugio::PinInterface> P : mNode->enumInputPins() )
	{
		bool		v = variant( P ).toBool();

		if( !OutHas )
		{
			OutVal = v;
			OutHas = true;
		}
		else
		{
			OutVal |= v;
		}
	}

	OutVal = !OutVal;

	if( OutVal != mValOutput->variant().toBool() )
	{
		mValOutput->setVariant( OutVal );

		pinUpdated( mPinOutput );
	}
}

QList<QUuid> NorNode::pinAddTypesInput() const
{
	static QList<QUuid>		PinLst;

	if( PinLst.isEmpty() )
	{
		PinLst << PID_FLOAT;
		PinLst << PID_INTEGER;
		PinLst << PID_STRING;
		PinLst << PID_BOOL;
	}

	return( PinLst );
}

bool NorNode::canAcceptPin(fugio::PinInterface *pPin) const
{
	if( pPin->direction() != PIN_OUTPUT )
	{
		return( false );
	}

	return( true );
}
