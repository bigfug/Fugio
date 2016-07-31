#ifndef MIDIOUTPUTHELPERNODE_H
#define MIDIOUTPUTHELPERNODE_H

#include <QObject>

#include <fugio/portmidi/uuid.h>

#include <fugio/nodecontrolbase.h>
#include <fugio/paired_pins_helper_interface.h>

class MidiOutputHelperNode : public fugio::NodeControlBase, public fugio::PairedPinsHelperInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::PairedPinsHelperInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Converts floating point inputs (0.0-1.0) to MIDI values (0-127)" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Midi_Output_Helper" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit MidiOutputHelperNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~MidiOutputHelperNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual QList<QUuid> pinAddTypesInput() const Q_DECL_OVERRIDE;

	virtual bool canAcceptPin(fugio::PinInterface *pPin) const Q_DECL_OVERRIDE;

	virtual bool pinShouldAutoRename( fugio::PinInterface *pPin ) const Q_DECL_OVERRIDE;

	//-------------------------------------------------------------------------
	// PairedPinsHelperInterface interface
public:
	virtual QUuid pairedPinControlUuid( QSharedPointer<fugio::PinInterface> pPin ) const Q_DECL_OVERRIDE;
};

#endif // MIDIOUTPUTHELPERNODE_H
