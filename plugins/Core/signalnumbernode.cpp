#include "signalnumbernode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/core/variant_interface.h>

#include <fugio/context_signals.h>

#include <qmath.h>

QMap<QString,SignalNumberNode::SignalType>		 SignalNumberNode::mSignalTypes;

SignalNumberNode::SignalNumberNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mSignalType( SINE ), mFrequency( 440.0 ), mVolume( 1.0 ), mOffset( 0.5 ), mBias( 0 )
{
	static const QUuid	PID_SIGNAL_TYPE   = QUuid( "{0030CB72-CCAA-43FC-86C4-5CF225C97BCC}" );
	static const QUuid	PID_SIGNAL_OFFSET = QUuid( "{496F6FA1-9DFB-4315-8186-EAAA643647EA}" );
	static const QUuid	PID_OUTPUT_VALUE  = QUuid( "{9BAF0D8C-7034-4462-9C1A-44C7A9D44527}" );
	FUGID( PIN_INPUT_BIAS, "51297977-7b4b-4e08-9dea-89a8add4abe0" )

	if( mSignalTypes.isEmpty() )
	{
		mSignalTypes.insert( "Sine",     SignalType::SINE );
		mSignalTypes.insert( "Square",   SignalType::SQUARE );
		mSignalTypes.insert( "Triangle", SignalType::TRIANGLE );
		mSignalTypes.insert( "Saw",      SignalType::SAW );
	}

	mPinSignalTypeChoice = pinInput<fugio::ChoiceInterface *>( tr( "Signal Type" ), mPinSignalType, PID_CHOICE, PID_SIGNAL_TYPE );

	mPinSignalType->setValue( mSignalTypes.key( SignalType::SINE ) );

	mPinSignalTypeChoice->setChoices( mSignalTypes.keys() );

	mPinInputFrequency = pinInput( tr( "Frequency (Hz)" ) );

	mPinInputFrequency->setValue( mFrequency );

	mPinInputVolume = pinInput( tr( "Volume" ) );

	mPinInputVolume->setValue( 1.0 );

	mPinInputOffset = pinInput( tr( "Offset" ), PID_SIGNAL_OFFSET );

	mPinInputOffset->setValue( 0.5 );

	mPinInputBias = pinInput( tr( "Bias" ), PIN_INPUT_BIAS );

	mPinInputBias->setValue( mBias );

	mValOutputValue = pinOutput<fugio::VariantInterface *>( "Number", mPinOutputValue, PID_FLOAT, PID_OUTPUT_VALUE );

	mPinInputFrequency->setDescription( tr( "The freqency to generate in hertz (Hz)" ) );

	mPinInputVolume->setDescription( tr( "The volume of the frequency (usually 0.0 - 1.0)" ) );
}

bool SignalNumberNode::initialise()
{
	if( !fugio::NodeControlBase::initialise() )
	{
		return( false );
	}

	connect( mNode->context()->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(onContextFrame(qint64)) );

	return( true );
}

bool SignalNumberNode::deinitialise()
{
	disconnect( mNode->context()->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(onContextFrame(qint64)) );

	return( NodeControlBase::deinitialise() );
}

void SignalNumberNode::onContextFrame( qint64 pTimeStamp )
{
	const double		 SamplesPerPhase = 48000.0 / mFrequency;
	qint64				 SamplePosition = ( pTimeStamp * 48000 ) / 1000;
	float				 Value = 0;

	const double	SmpPhs = fmod( double( SamplePosition ) / SamplesPerPhase, 1.0 );

	if( mSignalType == SINE )
	{
		Value = qSin( SmpPhs * 2.0 * M_PI );
	}
	else if( mSignalType == SQUARE )
	{
		Value = SmpPhs > mOffset ? -1.0 : 1.0;
	}
	else if( mSignalType == SAW )
	{
		Value = 2.0f * ( SmpPhs - qFloor( SmpPhs + 0.5f ) );
	}
	else if( mSignalType == TRIANGLE )
	{
		Value = 1.0f - qAbs( SmpPhs - 0.5f ) * 4.0f;
	}

	Value += mBias;

	Value *= mVolume;

	if( Value != mValOutputValue->variant().toFloat() )
	{
		mValOutputValue->setVariant( Value );

		pinUpdated( mPinOutputValue );
	}
}

void SignalNumberNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	mSignalType = mSignalTypes.value( variant( mPinSignalType ).toString() );

	mFrequency = variant( mPinInputFrequency ).toDouble();

	mVolume = std::max( 0.0, variant( mPinInputVolume ).toDouble() );

	mOffset = qBound( 0.0, variant( mPinInputOffset ).toDouble(), 1.0 );

	mBias = variant( mPinInputBias ).toDouble();
}

