#ifndef MIDIINPUTPIN_H
#define MIDIINPUTPIN_H

#include <fugio/core/uuid.h>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>
#include <fugio/pincontrolbase.h>

#include <fugio/midi/midi_input_interface.h>

class MidiInputPin : public fugio::PinControlBase, public fugio::MidiInputInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::MidiInputInterface )

public:
	Q_INVOKABLE explicit MidiInputPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~MidiInputPin( void ) {}

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE
	{
		return( "" );
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "MIDI Input Pin" );
	}

	//-------------------------------------------------------------------------
	// fugio::MidiInputInterface

	virtual void midiProcessInput( const fugio::MidiEvent *pMessages, quint32 pMessageCount ) Q_DECL_OVERRIDE;

private:
};

#endif // MIDIINPUTPIN_H
