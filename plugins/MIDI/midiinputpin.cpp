#include "midiinputpin.h"

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>

MidiInputPin::MidiInputPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin )
{

}

void MidiInputPin::midiProcessInput( const fugio::MidiEvent *pMessages, quint32 pMessageCount )
{
	fugio::MidiInputInterface	*MII = qobject_cast<fugio::MidiInputInterface *>( mPin->node()->control()->qobject() );

	if( MII )
	{
		MII->midiProcessInput( pMessages, pMessageCount );
	}
}
