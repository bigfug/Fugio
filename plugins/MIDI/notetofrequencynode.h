#ifndef NOTETOFREQUENCYNODE_H
#define NOTETOFREQUENCYNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>
#include <fugio/core/variant_interface.h>

class NoteToFrequencyNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Converts a MIDI note value to audio frequency value" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Midi_Note_To_Frequency" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE NoteToFrequencyNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~NoteToFrequencyNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputNote;
	QSharedPointer<fugio::PinInterface>			 mPinInputBend;
	QSharedPointer<fugio::PinInterface>			 mPinInputRange;

	QSharedPointer<fugio::PinInterface>			 mPinOutput;
	fugio::VariantInterface						*mValOutput;
};

#endif // NOTETOFREQUENCYNODE_H
