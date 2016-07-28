#ifndef MIDINOTENODE_H
#define MIDINOTENODE_H

#include <QObject>

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_interface.h>
#include <fugio/midi/midi_interface.h>

#include <fugio/core/uuid.h>

#include <fugio/nodecontrolbase.h>

class MidiNoteNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Input is MIDI note number (0-127), output is raw MIDI message" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Midi_Input" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit MidiNoteNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~MidiNoteNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual QList<QUuid> pinAddTypesInput() const Q_DECL_OVERRIDE;
	virtual bool canAcceptPin(fugio::PinInterface *pPin) const Q_DECL_OVERRIDE;

private:
	QSharedPointer<fugio::PinInterface>			 mPinChannel;

	QSharedPointer<fugio::PinInterface>			 mPinOutputMidi;
	fugio::MidiInterface						*mValOutputMidi;

	QMap<QUuid,int>								 mLastNote;
};

#endif // MIDINOTENODE_H
