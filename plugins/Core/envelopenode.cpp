#include "envelopenode.h"

#include <fugio/core/uuid.h>
#include <fugio/context_signals.h>
#include <fugio/node_signals.h>

#include <fugio/context_interface.h>

EnvelopeNode::EnvelopeNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mEnvState( START )
{
	static QUuid	PID_ATTACK  = QUuid( "{1BCC10EA-67B0-4B47-B42D-A34B912B4EAB}" );
	static QUuid	PID_DECAY   = QUuid( "{233FAA84-FCCC-4201-90E7-00E26CDAF82E}" );
	static QUuid	PID_RELEASE = QUuid( "{EC9EDF19-DCDB-4CB2-85AE-F799EF24CA3D}" );
	static QUuid	PID_SUSTAIN = QUuid( "{10AC9AA4-A883-468A-A64A-24804BD94991}" );

	mPinAttack  = pinInput( "Attack", PID_ATTACK );
	mPinDecay   = pinInput( "Decay", PID_DECAY );
	mPinSustain = pinInput( "Sustain", PID_SUSTAIN );
	mPinRelease = pinInput( "Release", PID_RELEASE );

	mPinAttack->setValue( 0.0f );
	mPinDecay->setValue( 0.0f );
	mPinSustain->setValue( 1.0f );
	mPinRelease->setValue( 0.0f );

	QSharedPointer<fugio::PinInterface>			 mPinInput;

	mPinInput = pinInput( "Input 1" );

	mPinInput->registerPinInputType( PID_BOOL );

	QSharedPointer<fugio::PinInterface>			 mPinOutput;

	pinOutput<fugio::VariantInterface *>( "Output 1", mPinOutput, PID_FLOAT );

	mNode->pairPins( mPinInput, mPinOutput );
}

bool EnvelopeNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	connect( mNode->context()->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(contextFrame(qint64)) );

	connect( mNode->qobject(), SIGNAL(pinAdded(QSharedPointer<fugio::NodeInterface>,QSharedPointer<fugio::PinInterface>)), this, SLOT(pinAdded(QSharedPointer<fugio::NodeInterface>,QSharedPointer<fugio::PinInterface>)) );

	return( true );
}

bool EnvelopeNode::deinitialise()
{
	disconnect( mNode->context()->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(contextFrame(qint64)) );

	return( NodeControlBase::deinitialise() );
}

QList<QUuid> EnvelopeNode::pinAddTypesInput() const
{
	QList<QUuid>	TypeList;

	TypeList << PID_BOOL;

	return( TypeList );
}

bool EnvelopeNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_OUTPUT );
}

QUuid EnvelopeNode::pairedPinControlUuid( QSharedPointer<fugio::PinInterface> pPin ) const
{
	return( pPin->direction() == PIN_OUTPUT ? PID_FLOAT : PID_BOOL );
}

void EnvelopeNode::contextFrame( qint64 pTimeStamp )
{
	for( fugio::NodeInterface::UuidPair UP : mNode->pairedPins() )
	{
		QSharedPointer<fugio::PinInterface> SrcPin = mNode->findPinByLocalId( UP.first );

		const bool		NewInputState = variant( SrcPin ).toBool();
		PinInf			NewPinInf = mPinInf.value( UP.first );

		if( NewInputState != NewPinInf.mInputState )
		{
			NewPinInf.mInputTime = pTimeStamp;

			NewPinInf.mValueAtSwitch = NewPinInf.mValue;

			mPinInf.insert( UP.first, NewPinInf );
		}
	}

	for( fugio::NodeInterface::UuidPair UP : mNode->pairedPins() )
	{
		QSharedPointer<fugio::PinInterface> SrcPin = mNode->findPinByLocalId( UP.first );
		QSharedPointer<fugio::PinInterface> DstPin = mNode->findPinByLocalId( UP.second );

		const bool		NewInputState = variant( SrcPin ).toBool();

		PinInf			CurPinInf = mPinInf.value( UP.first );

		if( NewInputState )
		{
			float			AttackVal  = variant( mPinAttack  ).toFloat();
			float			DecayVal   = variant( mPinDecay   ).toFloat();
			float			SustainVal = variant( mPinSustain ).toFloat();

			qint64			AttackStart  = CurPinInf.mInputTime;
			qint64			DecayStart   = AttackStart + qint64( AttackVal * 1000.0f );
			qint64			SustainStart = DecayStart  + qint64( DecayVal  * 1000.0f );

			if( pTimeStamp < DecayStart )
			{
				CurPinInf.mValue = CurPinInf.mValueAtSwitch + ( ( 1.0f - CurPinInf.mValueAtSwitch ) * ( float( pTimeStamp - AttackStart ) / float( DecayStart - AttackStart ) ) );
			}
			else if( pTimeStamp < SustainStart )
			{
				CurPinInf.mValue = qMax( SustainVal, 1.0f - ( float( pTimeStamp - DecayStart ) / float( SustainStart - DecayStart ) ) );
			}
			else
			{
				CurPinInf.mValue = SustainVal;
			}
		}
		else
		{
			float			ReleaseVal = variant( mPinRelease ).toFloat();

			qint64			ReleaseStart = CurPinInf.mInputTime;
			qint64			ReleaseEnd   = ReleaseStart + qint64( ReleaseVal * 1000.0f );

			if( pTimeStamp < ReleaseEnd )
			{
				CurPinInf.mValue = CurPinInf.mValueAtSwitch * ( 1.0f - ( float( pTimeStamp - ReleaseStart ) / float( ReleaseEnd - ReleaseStart ) ) );
			}
			else
			{
				CurPinInf.mValue = 0.0f;
			}
		}

		CurPinInf.mInputState = NewInputState;

		mPinInf.insert( UP.first, CurPinInf );

		if( fugio::VariantInterface *V = qobject_cast<fugio::VariantInterface *>( DstPin->control()->qobject() ) )
		{
			if( CurPinInf.mValue != V->variant().toFloat() )
			{
				V->setVariant( CurPinInf.mValue );

				pinUpdated( DstPin );
			}
		}
	}
}
