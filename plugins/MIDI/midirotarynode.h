#ifndef MIDIROTARYNODE_H
#define MIDIROTARYNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>
#include <fugio/core/variant_interface.h>

class MidiRotaryNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Processes input from a rotary MIDI control." )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Midi_Rotary_Control" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE MidiRotaryNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~MidiRotaryNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInput;
	QSharedPointer<fugio::PinInterface>			 mPinForce;

	QSharedPointer<fugio::PinInterface>			 mPinOutput;
	fugio::VariantInterface						*mValOutput;
};

#endif // MIDIROTARYNODE_H
