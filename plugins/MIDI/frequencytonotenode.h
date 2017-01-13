#ifndef FREQUENCYTONOTENODE_H
#define FREQUENCYTONOTENODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>
#include <fugio/core/variant_interface.h>

class FrequencyToNoteNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Converts a MIDI note value to audio frequency value" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Frequency_To_Node_(MIDI)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE FrequencyToNoteNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~FrequencyToNoteNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputFrequency;
	QSharedPointer<fugio::PinInterface>			 mPinInputRange;

	QSharedPointer<fugio::PinInterface>			 mPinOutputNote;
	fugio::VariantInterface						*mValOutputNote;

	QSharedPointer<fugio::PinInterface>			 mPinOutputBend;
	fugio::VariantInterface						*mValOutputBend;

	int											 mLastNote;
	float										 mLastFreq;
};

#endif // FREQUENCYTONOTENODE_H
