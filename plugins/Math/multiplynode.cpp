#include "multiplynode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>

MultiplyNode::MultiplyNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	static const QUuid	PII_NUMBER1( "{c13a41c6-544b-46bb-a9f2-19dd156d236c}" );
	static const QUuid	PII_NUMBER2( "{608ac771-490b-4ae6-9c81-12b9af526d09}" );

	pinInput( "Number", PII_NUMBER1 );
	pinInput( "Number", PII_NUMBER2 );

	mValOutput = pinOutput<fugio::VariantInterface *>( "Number", mPinOutput, PID_FLOAT );
}

void MultiplyNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	double		OutVal;
	bool		OutHas = false;

	for( QSharedPointer<fugio::PinInterface> P : mNode->enumInputPins() )
	{
		bool		b;
		double		v = variant( P ).toDouble( &b );

		if( b )
		{
			if( !OutHas )
			{
				OutVal = v;
				OutHas = true;
			}
			else
			{
				OutVal *= v;
			}
		}
	}

	if( OutHas && OutVal != mValOutput->variant().toDouble() )
	{
		mValOutput->setVariant( OutVal );

		pinUpdated( mPinOutput );
	}
}


QList<QUuid> MultiplyNode::pinAddTypesInput() const
{
	static QList<QUuid>		PinLst;

	if( PinLst.isEmpty() )
	{
		PinLst << PID_FLOAT;
		PinLst << PID_INTEGER;
		PinLst << PID_STRING;
	}

	return( PinLst );
}

bool MultiplyNode::canAcceptPin(fugio::PinInterface *pPin) const
{
	if( pPin->direction() != PIN_OUTPUT )
	{
		return( false );
	}

	return( true );
}

