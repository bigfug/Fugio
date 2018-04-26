#include "bitstointnode.h"

#include <fugio/pin_variant_iterator.h>

#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QSizeF>
#include <QQuaternion>
#include <QMatrix4x4>

#include <fugio/core/uuid.h>
#include <fugio/math/uuid.h>
#include <fugio/math/math_interface.h>

#include <fugio/pin_variant_iterator.h>

#include "mathplugin.h"

BitsToIntNode::BitsToIntNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_NUMBER1, "c13a41c6-544b-46bb-a9f2-19dd156d236c" );
	FUGID( PIN_OUTPUT_NUMBER, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );

	QSharedPointer<fugio::PinInterface>			 PinInput;
	QSharedPointer<fugio::PinInterface>			 PinOutput;

	PinInput = pinInput( tr( "Input" ), PIN_NUMBER1 );

	pinOutput<fugio::VariantInterface *>( tr( "Integer" ), PinOutput, PID_INTEGER, PIN_OUTPUT_NUMBER );

	mNode->pairPins( PinInput, PinOutput );
}

void BitsToIntNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	for( fugio::NodeInterface::UuidPair PP : mNode->pairedPins() )
	{
		QSharedPointer<fugio::PinInterface>			 SrcPin = mNode->findPinByLocalId( PP.first );
		QSharedPointer<fugio::PinInterface>			 DstPin = mNode->findPinByLocalId( PP.second );

		if( !SrcPin || !DstPin )
		{
			continue;
		}

		if( !SrcPin->isUpdated( pTimeStamp ) )
		{
			continue;
		}

		int			OutVal = 0;

		fugio::PinVariantIterator	PinItr( SrcPin );

		for( int i = 0 ; i < PinItr.count() ; i++ )
		{
			OutVal <<= 1;
			OutVal |= PinItr.index( i ).toBool();
		}

		if( fugio::VariantInterface *V = qobject_cast<fugio::VariantInterface *>( DstPin->control()->qobject() ) )
		{
			if( V->variant().toInt() != OutVal )
			{
				V->setVariant( OutVal );

				pinUpdated( DstPin );
			}
		}
	}
}

QList<QUuid> BitsToIntNode::pinAddTypesInput() const
{
	static QList<QUuid>		PinLst;

	if( PinLst.isEmpty() )
	{
		PinLst << PID_BOOL;
	}

	return( PinLst );
}

bool BitsToIntNode::canAcceptPin(fugio::PinInterface *pPin) const
{
	if( pPin->direction() != PIN_OUTPUT )
	{
		return( false );
	}

	return( true );
}

QUuid BitsToIntNode::pairedPinControlUuid(QSharedPointer<PinInterface> pPin) const
{
	return( PID_INTEGER );
}
