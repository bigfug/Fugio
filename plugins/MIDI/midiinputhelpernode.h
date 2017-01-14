#ifndef MIDIINPUTHELPER_H
#define MIDIINPUTHELPER_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

#include <fugio/paired_pins_helper_interface.h>

class MidiInputHelperNode : public fugio::NodeControlBase, public fugio::PairedPinsHelperInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::PairedPinsHelperInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Converts MIDI values (0-127) into floating point outputs (0.0-1.0)" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Midi_Input_Helper" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE MidiInputHelperNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~MidiInputHelperNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual QList<QUuid> pinAddTypesInput() const Q_DECL_OVERRIDE;

	virtual bool canAcceptPin(fugio::PinInterface *pPin) const Q_DECL_OVERRIDE;

	virtual bool pinShouldAutoRename(fugio::PinInterface *pPin) const Q_DECL_OVERRIDE;

	//-------------------------------------------------------------------------
	// PairedPinsHelperInterface interface
public:
	virtual QUuid pairedPinControlUuid(QSharedPointer<fugio::PinInterface> pPin) const Q_DECL_OVERRIDE;
};

#endif // MIDIINPUTHELPER_H
