#include "getsizenode.h"

#include <fugio/core/uuid.h>
#include <fugio/core/size_interface.h>

GetSizeNode::GetSizeNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_SOURCE,	"0898BEA5-A2AB-4216-BE9E-ACFCDB9502F3" );
	FUGID( PIN_OUTPUT_SIZE,		"DD67A091-B7C0-4F65-8DE5-DCBD7F66CE03" );

	QSharedPointer<fugio::PinInterface>		PinI, PinO;

	PinI = pinInput( "Input", PIN_INPUT_SOURCE );
	PinO = pinOutput( "Size", PID_SIZE, PIN_OUTPUT_SIZE );

	mNode->pairPins( PinI, PinO );
}

void GetSizeNode::inputsUpdated( qint64 pTimeStamp )
{
	for( fugio::NodeInterface::UuidPair UP : mNode->pairedPins() )
	{
		QSharedPointer<fugio::PinInterface>		 PinI = mNode->findPinByLocalId( UP.first );
		QSharedPointer<fugio::PinInterface>		 PinO = mNode->findPinByLocalId( UP.second );
		fugio::VariantInterface					*VarO = ( PinO && PinO->hasControl() ? qobject_cast<fugio::VariantInterface *>( PinO->control()->qobject() ) : nullptr );

		if( !PinI || !VarO )
		{
			continue;
		}

		if( !PinI->isUpdated( pTimeStamp ) )
		{
			continue;
		}

		fugio::SizeInterface	*SzeInt = input<fugio::SizeInterface *>( PinI );

		if( !SzeInt )
		{
			continue;
		}

		QVariant	Size;

		switch( SzeInt->sizeDimensions() )
		{
			case 1:
				Size = SzeInt->sizeWidth();

				if( PinO->controlUuid() != PID_FLOAT )
				{
					QSharedPointer<fugio::PinControlInterface> PCI = mNode->context()->global()->createPinControl( PID_FLOAT, PinO );

					if( PCI )
					{
						PinO->setControl( PCI );

						VarO = qobject_cast<fugio::VariantInterface *>( PCI->qobject() );
					}
				}
				break;

			case 2:
				Size = SzeInt->toSizeF();

				if( PinO->controlUuid() != PID_SIZE )
				{
					QSharedPointer<fugio::PinControlInterface> PCI = mNode->context()->global()->createPinControl( PID_SIZE, PinO );

					if( PCI )
					{
						PinO->setControl( PCI );

						VarO = qobject_cast<fugio::VariantInterface *>( PCI->qobject() );
					}
				}
				break;

			case 3:
				Size = SzeInt->toVector3D();

				if( PinO->controlUuid() != PID_SIZE_3D )
				{
					QSharedPointer<fugio::PinControlInterface> PCI = mNode->context()->global()->createPinControl( PID_SIZE_3D, PinO );

					if( PCI )
					{
						PinO->setControl( PCI );

						VarO = qobject_cast<fugio::VariantInterface *>( PCI->qobject() );
					}
				}
				break;
		}

		if( VarO->variant() != Size )
		{
			VarO->setVariant( Size );

			pinUpdated( PinO );
		}
	}
}

QList<QUuid> GetSizeNode::pinAddTypesInput() const
{
	return( QList<QUuid>{} );
}

bool GetSizeNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	if( pPin->direction() != PIN_OUTPUT )
	{
		return( false );
	}

	if( !pPin->hasControl() )
	{
		return( false );
	}

	fugio::SizeInterface	*SzeInt = qobject_cast<fugio::SizeInterface *>( pPin->control()->qobject() );

	if( !SzeInt )
	{
		return( false );
	}

	return( true );
}

bool GetSizeNode::pinShouldAutoRename( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_OUTPUT );
}

QUuid GetSizeNode::pairedPinControlUuid( QSharedPointer<fugio::PinInterface> pPin ) const
{
	return( pPin->direction() == PIN_OUTPUT ? PID_SIZE : QUuid() );
}
