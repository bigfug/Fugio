#include "smoothnode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/context_signals.h>
#include <fugio/node_signals.h>

SmoothNode::SmoothNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mLastTime( std::numeric_limits<qint64>::max() )
{
	mPinInput = pinInput( "Number" );

	mPinIncSpeed = pinInput( "Inc. Speed" );
	mPinDecSpeed = pinInput( "Dec. Speed" );

	mPinIncSpeed->setValue( 1.0 );
	mPinDecSpeed->setValue( 1.0 );

	mValOutput = pinOutput<fugio::VariantInterface *>( "Number", mPinOutput, PID_FLOAT );

	mPinInput->setAutoRename( true );

	mNode->pairPins( mPinInput, mPinOutput );

	mPinInput->setDescription( tr( "The input Number that we will aim towards" ) );

	mPinIncSpeed->setDescription( tr( "If input Number is higher than our current output Number, this is how much we will add per second" ) );

	mPinDecSpeed->setDescription( tr( "If input Number is lower than our current output Number, this is how much we will subtract per second" ) );

	mPinOutput->setDescription( tr( "The output Number" ) );
}

bool SmoothNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	connect( mNode->context()->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(onContextFrame(qint64)) );

	return( true );
}

bool SmoothNode::deinitialise()
{
	disconnect( mNode->context()->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(onContextFrame(qint64)) );

	return( NodeControlBase::deinitialise() );
}

void SmoothNode::onContextFrame( qint64 pTimeStamp )
{
	if( pTimeStamp < mLastTime )
	{
		mLastTime = pTimeStamp;
	}

	const qreal		IncVal = variant( mPinIncSpeed ).toReal();
	const qreal		DecVal = variant( mPinDecSpeed ).toReal();

	for( fugio::NodeInterface::UuidPair UP : mNode->pairedPins() )
	{
		QSharedPointer<fugio::PinInterface> SrcPin = mNode->findPinByLocalId( UP.first );
		QSharedPointer<fugio::PinInterface> DstPin = mNode->findPinByLocalId( UP.second );

		if( !SrcPin || !DstPin || !DstPin->hasControl() )
		{
			continue;
		}

		fugio::VariantInterface			*DstVar = qobject_cast<fugio::VariantInterface *>( DstPin->control()->qobject() );

		qreal		CurVal = variant( SrcPin ).toReal();
		qreal		OutVal = DstVar->variant().toReal();

		if( CurVal == OutVal )
		{
			continue;
		}

		qreal		AddVal = 0;

		AddVal = qreal( pTimeStamp - mLastTime ) / 1000.0;

		if( CurVal > OutVal )
		{
			AddVal *= IncVal;

			if( OutVal + AddVal > CurVal )
			{
				AddVal = CurVal - OutVal;
			}
		}
		else
		{
			AddVal *= -DecVal;

			if( OutVal + AddVal < CurVal )
			{
				AddVal = CurVal - OutVal;
			}
		}

		qreal		NewVal = OutVal + AddVal;

		if( DstVar->variant().toReal() != NewVal )
		{
			DstVar->setVariant( OutVal + AddVal );

			pinUpdated( DstPin );
		}
	}

	mLastTime = pTimeStamp;
}

QList<QUuid> SmoothNode::pinAddTypesInput() const
{
	static QList<QUuid>	PinLst =
	{
		PID_FLOAT
	};

	return( PinLst );
}

bool SmoothNode::canAcceptPin(fugio::PinInterface *pPin) const
{
	return( pPin->direction() == PIN_OUTPUT );
}

bool SmoothNode::pinShouldAutoRename( fugio::PinInterface *pPin ) const
{
	Q_UNUSED( pPin )

	return( true );
}

QUuid SmoothNode::pairedPinControlUuid( QSharedPointer<fugio::PinInterface> pPin ) const
{
	Q_UNUSED( pPin )

	return( PID_FLOAT );
}
