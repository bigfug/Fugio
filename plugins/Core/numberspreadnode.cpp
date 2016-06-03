#include "numberspreadnode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>

#include <qmath.h>

NumberSpreadNode::NumberSpreadNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mPinValue  = pinInput( "Number" );
	mPinCenter = pinInput( "Center" );
	mPinSpread = pinInput( "Spread" );

	mPinValue->setValue( 1.0f );
	mPinSpread->setValue( 1.0f );

	QSharedPointer<fugio::PinInterface>		P;

	pinOutput<fugio::PinControlInterface *>( "Number", P, PID_FLOAT );
}

void NumberSpreadNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	const float		Spread = variant( mPinSpread ).toFloat();
	const float		Center = variant( mPinCenter ).toFloat();
	const float		Value  = variant( mPinValue  ).toFloat();

	QList<QSharedPointer<fugio::PinInterface>>		PinLst = mNode->enumOutputPins();

	for( int i = 0 ; i < PinLst.size() ; i++ )
	{
		QSharedPointer<fugio::PinInterface>		P = PinLst[ i ];

		if( !P->hasControl() )
		{
			continue;
		}

		float		PinBeg = float( i + 0 ) / float( PinLst.size() - 1 );
//		float		PinEnd = float( i + 1 ) / float( PinLst.size() - 1 );
		float		PinVal = 0.0f;

//		if( PinBeg <= Center && PinEnd >= Center )
//		{
//			PinVal = Value;
//		}
//		else
		{
			float		PinPos = ( Center - PinBeg ) * ( M_PI * Spread );

			if( PinPos >= -M_PI && PinPos <= M_PI )
			{
				PinVal = qCos( PinPos ) * Value;
			}
		}

		if( fugio::VariantInterface *I = qobject_cast<fugio::VariantInterface *>( P->control()->qobject() ) )
		{
			I->setVariant( PinVal );

			pinUpdated( P );
		}
	}
}

QList<QUuid> NumberSpreadNode::pinAddTypesOutput() const
{
	QList<QUuid>		PinLst;

	PinLst << PID_FLOAT;

	return( PinLst );
}

bool NumberSpreadNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_INPUT );
}
