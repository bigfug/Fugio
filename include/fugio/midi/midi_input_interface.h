#ifndef MIDI_INPUT_INTERFACE_H
#define MIDI_INPUT_INTERFACE_H

#include <fugio/global.h>
#include <fugio/midi/midi_interface.h>

FUGIO_NAMESPACE_BEGIN

class MidiInputInterface
{
public:
	virtual ~MidiInputInterface( void ) {}

	virtual void midiProcessInput( const MidiEvent *pMessages, quint32 pMessageCount ) = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::MidiInputInterface, "com.bigfug.fugio.midi-input/1.0" )

#endif // MIDI_INPUT_INTERFACE_H
