#include "portmidioutputnode.h"

#include "devicemidi.h"

#include <QComboBox>

#include <fugio/node_interface.h>
#include <fugio/context_interface.h>
#include <fugio/core/variant_interface.h>
#include <fugio/midi/uuid.h>
#include <fugio/midi/midi_interface.h>

#include <fugio/context_signals.h>

#include <fugio/core/uuid.h>

#include <fugio/utils.h>

#include <fugio/node_signals.h>

PortMidiOutputNode::PortMidiOutputNode( QSharedPointer<fugio::NodeInterface> pNode ) :
	NodeControlBase( pNode ), mDevice( 0 ), mClockValue( 0 ), mClockLast( 0 ), mPlayState( false )
{
	mPinClock = pinInput( "Beat Position" );

	mPinMidi  = pinInput( "MIDI" );

	mPinMidi->registerPinInputType( PID_MIDI_OUTPUT );

	rebuildDeviceList();
}

bool PortMidiOutputNode::initialise( void )
{
	if( !fugio::NodeControlBase::initialise() )
	{
		return( false );
	}

	connect( mNode->context()->qobject(), SIGNAL(frameProcess()), this, SLOT(onFrameStart()) );
	connect( mNode->context()->qobject(), SIGNAL(frameEnd(qint64)), this, SLOT(onFrameEnd(qint64)) );

#if defined( PORTMIDI_SUPPORTED )
	if( mDeviceName.isEmpty() )
	{
		mDeviceName = mDeviceList.at( 1 );
	}

	midiDeviceSelected( mDeviceName );

	return( true );
#else
	mNode->setStatus( fugio::NodeInterface::Error );
	mNode->setStatusMessage( "No PortMidi Support" );

	return( false );
#endif
}

bool PortMidiOutputNode::deinitialise()
{
	mNode->context()->qobject()->disconnect( this );

	mDevice.clear();

	return( NodeControlBase::deinitialise() );
}

void PortMidiOutputNode::onFrameStart()
{
	if( !mDevice )
	{
		return;
	}

#if defined( PORTMIDI_SUPPORTED )
	PmEvent	E;

	E.timestamp = 0;

	if( mNode->context()->isPlaying() )
	{
		if( !mPlayState )
		{
			if( mNode->context()->position() == 0.0 )
			{
				E.message = Pm_Message( MIDI_START, 0, 0 );
			}
			else
			{
				E.message = Pm_Message( MIDI_CONTINUE, 0, 0 );
			}

			mDevice->output( E );
		}
	}
	else
	{
		if( mPlayState )
		{
			PmEvent	E;

			E.message   = Pm_Message( MIDI_STOP, 0, 0 );
			E.timestamp = 0;

			mDevice->output( E );
		}
	}
#endif

	mPlayState = mNode->context()->isPlaying();
}

void PortMidiOutputNode::onFrameEnd( qint64 pTimeStamp )
{
	if( !mDevice )
	{
		return;
	}

	if( mPinClock->isConnected() && mPinClock->isUpdated( pTimeStamp ) )
	{
		fugio::VariantInterface	*V = qobject_cast<fugio::VariantInterface *>( mPinClock->connectedPin()->control()->qobject() );

		if( V )
		{
#if defined( PORTMIDI_SUPPORTED )
			PmEvent	E;
			qreal	C = V->variant().toDouble();
			qreal	I = 1./24.;

			E.message   = Pm_Message( MIDI_CLOCK, 0, 0 );
			E.timestamp = 0;

			if( !mNode->context()->isPlaying() )
			{
				mClockValue = C / I;
			}
			else
			{
				if( std::abs( ( mClockValue * I ) - C ) > 0.25 )
				{
					mClockValue = C / I;
				}

				while( C > mClockValue * I )
				{
					mDevice->output( E );

					mClockValue += 1;
				}
			}
#endif
		}
	}

	for( QSharedPointer<fugio::PinInterface> P : mNode->enumInputPins() )
	{
		if( P == mPinClock )
		{
			continue;
		}

		if( !P->isUpdated( pTimeStamp ) )
		{
			continue;
		}

		fugio::MidiInterface		*M = input<fugio::MidiInterface *>( P );

		if( M )
		{
			mDevice->output( M->messages() );

			continue;
		}

//		fugio::ArrayInterface		*A = input<fugio::ArrayInterface *>( P );

//		if( A )
//		{
//			if( A->type() == QMetaType::Int && A->size() == 1 && A->stride() == sizeof( int ) )
//			{
//				mDevice->output( (const int32_t *)A->array(), A->count() );
//			}

//			continue;
//		}

		fugio::VariantInterface		*V = input<fugio::VariantInterface *>( P );

		if( V )
		{
			if( P->controlUuid() == PID_BYTEARRAY )
			{
				for( int i = 0 ; i < V->variantCount() ; i++ )
				{
					mDevice->outputSysEx( V->variant( i ).toByteArray() );
				}
			}
			else if( P->controlUuid() == PID_INTEGER )
			{
				mDevice->output( (const int32_t *)V->variantArray(), V->variantCount() * V->variantElementCount() );
			}
		}
	}
}

void PortMidiOutputNode::midiDeviceSelected( const QString &pDeviceName )
{
	if( mDeviceName == pDeviceName && mDevice )
	{
		return;
	}

	mDevice.clear();

	mDeviceName = pDeviceName;

	emit midiDeviceChanged( mDeviceName );

#if defined( PORTMIDI_SUPPORTED )
	const int	DevNamIdx = mDeviceList.indexOf( mDeviceName );

	if( DevNamIdx == 0 )
	{
		mNode->setStatus( fugio::NodeInterface::Initialised );
	}
	else
	{
		PmDeviceID		DevIdx;

		if( DevNamIdx == 1 )
		{
			DevIdx = Pm_GetDefaultOutputDeviceID();
		}
		else
		{
			DevIdx = DeviceMidi::deviceOutputNameIndex( mDeviceName );
		}

		if( DevIdx == pmNoDevice )
		{
			mNode->setStatus( fugio::NodeInterface::Warning );
		}

		if( ( mDevice = DeviceMidi::newDevice( DevIdx ) ) )
		{
			mNode->setStatus( fugio::NodeInterface::Initialised );
		}
	}
#endif
}

void PortMidiOutputNode::rebuildDeviceList()
{
	mDeviceList.clear();

	mDeviceList << tr( "None" );
	mDeviceList << tr( "Default Midi Output" );

#if defined( PORTMIDI_SUPPORTED )
	mDeviceList << DeviceMidi::deviceOutputNames();
#endif
}

QWidget *PortMidiOutputNode::gui()
{
	QComboBox							*GUI;

	if( ( GUI = new QComboBox() ) != nullptr )
	{
		GUI->addItems( mDeviceList );

		GUI->setCurrentText( mDeviceName );

		connect( GUI, SIGNAL(currentIndexChanged(QString)), this, SLOT(midiDeviceSelected(QString)) );

		connect( this, SIGNAL(midiDeviceChanged(QString)), GUI, SLOT(setCurrentText(QString)) );
	}

	return( GUI );
}

void PortMidiOutputNode::loadSettings( QSettings &pSettings )
{
	rebuildDeviceList();

	int		Index = pSettings.value( "index", 1 ).toInt();

	QString	DeviceName = pSettings.value( "device", mDeviceName ).toString();

	if( Index >= 0 && Index <= 1 )
	{
		DeviceName = mDeviceList[ Index ];
	}

	midiDeviceSelected( DeviceName );
}

void PortMidiOutputNode::saveSettings( QSettings &pSettings ) const
{
	pSettings.setValue( "device", mDeviceName );
	pSettings.setValue( "index", mDeviceList.indexOf( mDeviceName ) );
}

QList<QUuid> PortMidiOutputNode::pinAddTypesInput() const
{
	static QList<QUuid>	PinLst;

	if( PinLst.isEmpty() )
	{
		PinLst << PID_MIDI_OUTPUT;
		PinLst << PID_BYTEARRAY;
	}

	return( PinLst );
}

bool PortMidiOutputNode::canAcceptPin(fugio::PinInterface *pPin) const
{
	return( pPin->direction() == PIN_OUTPUT );
}
