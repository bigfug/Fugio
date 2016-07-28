#include "midioutputpin.h"

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>

MidiOutputPin::MidiOutputPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin )
{

}

void MidiOutputPin::addMessages( const QVector<fugio::MidiEvent> &pMessages )
{
	mMessages << pMessages;
}

QVector<fugio::MidiEvent> MidiOutputPin::messages() const
{
	return( mMessages );
}

QByteArray MidiOutputPin::systemExclusive() const
{
	return( mSysEx );
}

void MidiOutputPin::clearData()
{
	mMessages.clear();

	mSysEx.clear();
}
