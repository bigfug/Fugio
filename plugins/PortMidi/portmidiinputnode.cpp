#include "portmidiinputnode.h"

#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>

#include "devicemidi.h"

#include <fugio/core/uuid.h>

#include <fugio/node_interface.h>
#include <fugio/context_interface.h>
#include <fugio/context_signals.h>

#include <fugio/midi/uuid.h>
#include <fugio/midi/midi_interface.h>
#include <fugio/core/variant_interface.h>

#include <fugio/utils.h>

#include <fugio/performance.h>

#include "portmidiplugin.h"

PortMidiInputNode::PortMidiInputNode( QSharedPointer<fugio::NodeInterface> pNode ) :
	NodeControlBase( pNode )
{
	FUGID( PIN_OUTPUT_MIDI, "b2987601-db44-4fa8-a90d-ffc37864a2e3" );

	mValMidi = pinOutput<fugio::MidiInterface *>( "MIDI", mPinMidi, PID_MIDI_OUTPUT, PIN_OUTPUT_MIDI );

	rebuildDeviceList();
}

bool PortMidiInputNode::initialise( void )
{
	if( !fugio::NodeControlBase::initialise() )
	{
		return( false );
	}

	if( mDeviceName.isEmpty() )
	{
		mDeviceName = mDeviceList.at( 1 );
	}

	midiDeviceSelected( mDeviceName );

	return( true );
}

bool PortMidiInputNode::deinitialise()
{
	if( mDevice )
	{
		mDevice->unregisterMidiInputNode( this );

		mDevice.clear();
	}

	return( NodeControlBase::deinitialise() );
}

void PortMidiInputNode::midiDeviceSelected( const QString &pDeviceName )
{
	if( pDeviceName == mDeviceName && mDevice )
	{
		return;
	}

	if( mDevice )
	{
		mDevice->unregisterMidiInputNode( this );

		mDevice.clear();
	}

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
			DevIdx = Pm_GetDefaultInputDeviceID();
		}
		else
		{
			DevIdx = DeviceMidi::deviceInputNameIndex( mDeviceName );
		}

		if( DevIdx == pmNoDevice )
		{
			mNode->setStatus( fugio::NodeInterface::Warning );
		}

		if( ( mDevice = DeviceMidi::newDevice( DevIdx ) ) )
		{
			mDevice->registerMidiInputNode( this );

			mNode->setStatus( fugio::NodeInterface::Initialised );
		}
	}

	mNode->setStatus( mDevice && mDevice->isActive() ? fugio::NodeInterface::Initialised : fugio::NodeInterface::Warning );
#endif

	pinUpdated( mPinMidi );
}

void PortMidiInputNode::rebuildDeviceList()
{
	mDeviceList.clear();

	mDeviceList << tr( "None" );
	mDeviceList << tr( "Default Midi Input" );

#if defined( PORTMIDI_SUPPORTED )
	mDeviceList << DeviceMidi::deviceInputNames();
#endif
}

QWidget *PortMidiInputNode::gui( void )
{
	QComboBox			*GUI = new QComboBox();

	if( GUI )
	{
		GUI->addItems( mDeviceList );

		GUI->setCurrentText( mDeviceName );

		connect( GUI, SIGNAL(currentIndexChanged(QString)), this, SLOT(midiDeviceSelected(QString)) );

		connect( this, SIGNAL(midiDeviceChanged(QString)), GUI, SLOT(setCurrentText(QString)) );
	}

	return( GUI );
}

void PortMidiInputNode::loadSettings( QSettings &pSettings )
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

void PortMidiInputNode::saveSettings( QSettings &pSettings ) const
{
	pSettings.setValue( "device", mDeviceName );
	pSettings.setValue( "index", mDeviceList.indexOf( mDeviceName ) );
}

void PortMidiInputNode::midiProcessInput( const fugio::MidiEvent *pMessages, quint32 pMessageCount )
{
	fugio::Performance		Perf( mNode, "midiProcessInput", PortMidiPlugin::app()->timestamp() );

	QList<fugio::MidiInputInterface *>	NodLst = outputs<fugio::MidiInputInterface *>( mPinMidi );

	for( fugio::MidiInputInterface *MII : NodLst )
	{
		MII->midiProcessInput( pMessages, pMessageCount );
	}
}
