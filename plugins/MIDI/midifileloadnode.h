#ifndef MIDIFILELOADNODE_H
#define MIDIFILELOADNODE_H

#include <QObject>
#include <QBuffer>

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_interface.h>
#include <fugio/midi/midi_interface.h>

#include <fugio/core/uuid.h>

#include <fugio/nodecontrolbase.h>

#include "import/midifile.h"

class MidiFileLoadNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Loads a MIDI file" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Midi_File_Load" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit MidiFileLoadNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~MidiFileLoadNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual bool initialise() Q_DECL_OVERRIDE;
	virtual bool deinitialise() Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

private slots:
	void contextFrame( qint64 pTimeStamp );

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputFileData;

	QSharedPointer<fugio::PinInterface>			 mPinOutputMidi;
	fugio::MidiInterface						*mValOutputMidi;

	qint64										 mTimeOffset;
	qint64										 mTimeLast;

	MidiFile									 mMidiFile;
};

#endif // MIDIFILELOADNODE_H
