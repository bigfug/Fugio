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
}

bool PortMidiInputNode::initialise( void )
{
	if( !fugio::NodeControlBase::initialise() )
	{
		return( false );
	}

	return( true );
}

bool PortMidiInputNode::deinitialise()
{
	if( mDevice )
	{
		mDevice->unregisterMidiInputNode( this );
	}

	return( NodeControlBase::deinitialise() );
}

void PortMidiInputNode::setDeviceName( const QString &pDeviceName )
{
	if( mDevice )
	{
		mDevice->unregisterMidiInputNode( this );
	}

	mDevice = DeviceMidi::findDeviceInput( pDeviceName );

	if( mDevice )
	{
		mDevice->registerMidiInputNode( this );
	}

	mDeviceName = pDeviceName;

	mNode->setStatus( mDevice && mDevice->isActive() ? fugio::NodeInterface::Initialised : fugio::NodeInterface::Warning );
}

QWidget *PortMidiInputNode::gui( void )
{
	QComboBox			*GUI = new QComboBox();

	if( GUI )
	{
		QStringList			DevLst = DeviceMidi::deviceInputNames();

		if( !mDeviceName.isEmpty() && !DevLst.contains( mDeviceName ) )
		{
			DevLst << mDeviceName;

			std::sort( DevLst.begin(), DevLst.end() );
		}

		DevLst.prepend( tr( "<None>" ) );

		for( int i = 0 ; i < DevLst.size() ; i++ )
		{
			GUI->addItem( DevLst[ i ] );

			if( !mDeviceName.isEmpty() && mDeviceName == DevLst[ i ] )
			{
				GUI->setCurrentIndex( i );
			}
		}

		connect( GUI, SIGNAL(currentIndexChanged(QString)), this, SLOT(setDeviceName(QString)) );

		connect( this, SIGNAL(deviceChanged(QString)), GUI, SLOT(setCurrentText(QString)) );
	}

	return( GUI );
}

void PortMidiInputNode::loadSettings( QSettings &pSettings )
{
	QString		NewDevNam = pSettings.value( "device-name", mDeviceName ).toString();

	if( NewDevNam != mDeviceName )
	{
		setDeviceName( NewDevNam );

		emit deviceChanged( mDeviceName );
	}

	mNode->setStatus( mDevice && mDevice->isActive() ? fugio::NodeInterface::Initialised : fugio::NodeInterface::Warning );
}

void PortMidiInputNode::saveSettings( QSettings &pSettings ) const
{
	pSettings.setValue( "device-name", mDeviceName );
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
