#include "lastupdatedinputnode.h"

#include <fugio/global_interface.h>
#include <fugio/context_interface.h>
#include <fugio/core/uuid.h>

#include <QDoubleSpinBox>
#include <QDateTime>
#include <QSettings>
#include <QFormLayout>

#include <limits>

LastUpdatedInputNode::LastUpdatedInputNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mPinInput = pinInput( "Input 1" );

	mPinOutput = pinOutput( "Output", next_uuid() );
}

void LastUpdatedInputNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	QSharedPointer<fugio::PinControlInterface>		InpPin;
	qint64											InpTim = 0;

	for( QSharedPointer<fugio::PinInterface> P : mNode->enumInputPins() )
	{
		if( P->updated() > InpTim && P->isConnected() && P->connectedPin()->hasControl()  )
		{
			InpPin = P->connectedPin()->control();

			InpTim = P->updated();
		}
	}

	if( InpPin )
	{
		mPinOutput->setControl( InpPin );

		pinUpdated( mPinOutput );
	}
}


QList<QUuid> LastUpdatedInputNode::pinAddTypesInput() const
{
	return( mNode->context()->global()->pinIds().keys() );
}

bool LastUpdatedInputNode::canAcceptPin(fugio::PinInterface *pPin) const
{
	return( pPin->direction() == PIN_OUTPUT );
}
