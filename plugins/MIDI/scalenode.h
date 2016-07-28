#ifndef SCALENODE_H
#define SCALENODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>
#include <fugio/core/variant_interface.h>
#include <fugio/choice_interface.h>
#include <fugio/paired_pins_helper_interface.h>

class ScaleNode : public fugio::NodeControlBase, public fugio::PairedPinsHelperInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::PairedPinsHelperInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Midi_Note_To_Frequency" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE ScaleNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~ScaleNode( void ) {}

	// NodeControlInterface interface
public:
	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual QList<QUuid> pinAddTypesInput() const Q_DECL_OVERRIDE;

	virtual bool canAcceptPin(fugio::PinInterface *pPin) const Q_DECL_OVERRIDE;

	// PairedPinsHelperInterface interface

public:
	virtual QUuid pairedPinControlUuid( QSharedPointer<fugio::PinInterface> pPin ) const Q_DECL_OVERRIDE;

protected:
	typedef enum
	{
		AEOLIAN,
		BLUES,
		CHROMATIC,
		DIATONIC_MINOR,
		DORIAN,
		HARMONIC_MINOR,
		INDIAN,
		LOCRIAN,
		LYDIAN,
		MAJOR,
		MELODIC_MINOR,
		MINOR,
		MIXOLYDIAN,
		NATURAL_MINOR,
		PENTATONIC,
		PHRYGIAN,
		TURKISH
	} Scale;

	static int scaleLimit( int NoteOffset, ScaleNode::Scale MidiScale, int BaseNote, int NoteOutput);

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputKey;
	QSharedPointer<fugio::PinInterface>			 mPinInputScale;
	QSharedPointer<fugio::PinInterface>			 mPinInputNote;

	fugio::ChoiceInterface						*mValInputKey;
	fugio::ChoiceInterface						*mValInputScale;

	QSharedPointer<fugio::PinInterface>			 mPinOutputNote;
	fugio::VariantInterface						*mValOutputNote;

	static QMap<QString,int>					 mKeyMap;
	static QMap<QString,Scale>					 mScaleMap;
};

#endif // SCALENODE_H
