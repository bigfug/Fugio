#ifndef CHANNELINPUTNODE_H
#define CHANNELINPUTNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>
#include <fugio/core/variant_interface.h>
#include <fugio/midi/midi_interface.h>
#include <fugio/midi/midi_input_interface.h>

class ChannelInputNode : public fugio::NodeControlBase, public fugio::MidiInputInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::MidiInputInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Midi_Channel" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE ChannelInputNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~ChannelInputNode( void ) {}

	// NodeControlInterface interface
public:
	virtual bool initialise() Q_DECL_OVERRIDE;

	virtual QWidget *gui() Q_DECL_OVERRIDE;

	virtual QList<AvailablePinEntry> availableOutputPins() const Q_DECL_OVERRIDE;
	virtual bool mustChooseNamedOutputPin() const Q_DECL_OVERRIDE;

	// MidiInputInterface interface
public:
	virtual void midiProcessInput( const fugio::MidiEvent *pMessages, quint32 pMessageCount ) Q_DECL_OVERRIDE;

private slots:
	void setListenState( bool pChecked );

signals:
	void listenState( bool pChecked );

private:
	static unsigned short CombineBytes(unsigned char First, unsigned char Second)
	{
		unsigned short _14bit;
		_14bit = (unsigned short)Second;
		_14bit <<= 7;
		_14bit |= (unsigned short)First;
		return(_14bit);
	}

	typedef struct MidiPinPair
	{
		QSharedPointer<fugio::PinInterface>		mPin;
		fugio::VariantInterface					*mVal;

		MidiPinPair( void ) : mVal( nullptr )
		{

		}
	} MidiPinPair;

	typedef QMap<quint8,MidiPinPair>			 MidiEntryMap;

	MidiEntryMap::iterator findOrCreatePin( const QString &pName, MidiEntryMap &pEntryMap, const quint8 pEntryIndex, const bool pListening, bool &pSortPins, const QUuid &pPinType );

	void createMidiPin(MidiPinPair &MPP, const QString &pName, const QUuid &pPinType);

	void pinToMidiEntry( QSharedPointer<fugio::PinInterface> P, MidiPinPair &MPP );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputMidi;
	fugio::MidiInputInterface					*mValInputMidi;

	qint64										 mLastUpdate;
	bool										 mListen;

	QMap<quint8,quint8>							 mNoteMap;

	MidiEntryMap								 mNotePins;

	MidiEntryMap								 mNoteOnPins;
	MidiEntryMap								 mNoteOffPins;
	MidiEntryMap								 mNotePressurePins;

	MidiEntryMap								 mControlsPins;
	MidiEntryMap								 mProgramPins;
	MidiEntryMap								 mChannelPressurePins;

	MidiPinPair									 mNotesActive;
	MidiPinPair									 mNoteValue;
	MidiPinPair									 mPitchBend;
};

#endif // CHANNELINPUTNODE_H
