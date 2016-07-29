#include "portmidiplugin.h"

#include "devicemidi.h"

#include <fugio/global_interface.h>
#include <fugio/global_signals.h>

#include "portmidiinputnode.h"
#include "portmidioutputnode.h"

PortMidiPlugin	*PortMidiPlugin::mInstance = nullptr;

QList<QUuid>	NodeControlBase::PID_UUID;

ClassEntry	NodeClasses[] =
{
	ClassEntry( "MIDI Input",			"PortMidi", ClassEntry::None, NID_PORTMIDI_INPUT,  &PortMidiInputNode::staticMetaObject ),
	ClassEntry( "MIDI Output",			"PortMidi", ClassEntry::None, NID_PORTMIDI_OUTPUT, &PortMidiOutputNode::staticMetaObject ),
	ClassEntry()
};

ClassEntry PinClasses[] =
{
	ClassEntry()
};

PortMidiPlugin::PortMidiPlugin( void )
{
	mInstance = this;
}

PluginInterface::InitResult PortMidiPlugin::initialise( fugio::GlobalInterface *pApp )
{
	mApp = pApp;

	mApp->registerNodeClasses( NodeClasses );

	mApp->registerPinClasses( PinClasses );

	if( !DeviceMidi::deviceInitialise() )
	{
		return( INIT_FAILED );
	}

	connect( mApp->qobject(), SIGNAL(frameInitialise()), this, SLOT(onGlobalFrameInitialise()) );
	connect( mApp->qobject(), SIGNAL(frameEnd()), this, SLOT(onGlobalFrameEnd()) );

	return( INIT_OK );
}

void PortMidiPlugin::deinitialise( void )
{
	mApp->unregisterPinClasses( PinClasses );

	mApp->unregisterNodeClasses( NodeClasses );

	DeviceMidi::deviceDeinitialise();

	mApp = 0;
}

void PortMidiPlugin::onGlobalFrameInitialise()
{
	DeviceMidi::devicePacketStart();
}

void PortMidiPlugin::onGlobalFrameEnd( void )
{
	DeviceMidi::devicePacketEnd();
}
