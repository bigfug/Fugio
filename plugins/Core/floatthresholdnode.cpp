#include "floatthresholdnode.h"

#include <fugio/core/uuid.h>
#include <fugio/node_signals.h>

FloatThresholdNode::FloatThresholdNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_THRESHOLD,	"0898BEA5-A2AB-4216-BE9E-ACFCDB9502F3" );
	FUGID( PIN_INPUT_FLOAT,		"8B572C7B-42CE-4EE9-A2C2-9C90D004EE1D" );
	FUGID( PIN_OUTPUT_BOOL,		"DD67A091-B7C0-4F65-8DE5-DCBD7F66CE03" );

	mPinInputThreshold = pinInput( "Threshold", PIN_INPUT_THRESHOLD );

	QSharedPointer<fugio::PinInterface>		PinI, PinO;

	PinI = pinInput( "Float", PIN_INPUT_FLOAT );
	PinO = pinOutput( "Bool", PID_BOOL, PIN_OUTPUT_BOOL );

	mNode->pairPins( PinI, PinO );
}

void FloatThresholdNode::inputsUpdated( qint64 pTimeStamp )
{
	bool			ThresholdOk;
	const float		Threshold = qBound<float>( 0, variant( mPinInputThreshold ).toFloat( &ThresholdOk ), 1 );

	if( !ThresholdOk )
	{
		mNode->setStatus( fugio::NodeInterface::Error );
		mNode->setStatusMessage( "Threshold is not valid" );

		return;
	}

	mNode->setStatus( fugio::NodeInterface::Initialised );
	mNode->setStatusMessage( QString() );

	for( fugio::NodeInterface::UuidPair UP : mNode->pairedPins() )
	{
		QSharedPointer<fugio::PinInterface>		 PinI = mNode->findPinByLocalId( UP.first );
		QSharedPointer<fugio::PinInterface>		 PinO = mNode->findPinByLocalId( UP.second );
		fugio::VariantInterface					*VarO = ( PinO && PinO->hasControl() ? qobject_cast<fugio::VariantInterface *>( PinO->control()->qobject() ) : nullptr );

		if( !PinI || !VarO )
		{
			continue;
		}

		const float			PinVal = variant( PinI ).toFloat();
		const bool			PinRes = PinVal >= Threshold;

		if( VarO->variant().toBool() != PinRes )
		{
			VarO->setVariant( PinRes );

			pinUpdated( PinO );
		}
	}
}

QList<QUuid> FloatThresholdNode::pinAddTypesInput() const
{
	static QList<QUuid> PinLst =
	{
		PID_FLOAT
	};

	return( PinLst );
}

bool FloatThresholdNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	Q_UNUSED( pPin )

	return( true );
}

bool FloatThresholdNode::pinShouldAutoRename( fugio::PinInterface *pPin ) const
{
	return( pPin->localId() != mPinInputThreshold->localId() );
}

QUuid FloatThresholdNode::pairedPinControlUuid( QSharedPointer<fugio::PinInterface> pPin ) const
{
	return( pPin->direction() == PIN_OUTPUT ? PID_FLOAT : PID_BOOL );
}
