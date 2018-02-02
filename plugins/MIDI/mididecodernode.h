#ifndef MIDIDECODERNODE_H
#define MIDIDECODERNODE_H

#include <fugio/nodecontrolbase.h>
#include <fugio/midi/midi_input_interface.h>
#include <fugio/midi/midi_interface.h>

class MidiDecoderNode : public fugio::NodeControlBase, public fugio::MidiInputInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::MidiInputInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Processes incoming MIDI messages" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Midi_Decoder" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE MidiDecoderNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~MidiDecoderNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface interface

public:
	virtual QList<AvailablePinEntry> availableOutputPins() const Q_DECL_OVERRIDE;
	virtual bool mustChooseNamedOutputPin() const Q_DECL_OVERRIDE;
	virtual QList<QUuid> pinAddTypesOutput() const Q_DECL_OVERRIDE;

	//-------------------------------------------------------------------------
	// MidiInputInterface interface

public:
	virtual void midiProcessInput( const fugio::MidiEvent *pMessages, quint32 pMessageCount ) Q_DECL_OVERRIDE;

private slots:
	void pinAdded( QSharedPointer<fugio::PinInterface> P );

private:
	QString channelName( int pChannel ) const;

	void clearSysEx( void )
	{
		if( !mSysExCurrent.isEmpty() )
		{
			mValOutputSysEx->variantAppend( mSysExCurrent );

			mSysExCurrent.clear();
		}

		mSysEx = false;
	}

	void addSysEx( const int s, const fugio::MidiEvent e )
	{
		for( int i = s ; mSysEx && i < 4 ; i++ )
		{
			quint8	b = quint8( e.message >> ( i * 8 ) );

			if( b == 0xf7 )
			{
				processSysEx();
			}
			else
			{
				mSysExCurrent.append( b );
			}
		}

		mSysExEvents.append( e );
	}

	void processSysEx( void );

	void processChannelMessage( const fugio::MidiEvent &pMidiEvent );

	void processSystemMessage( const quint8 pMsgStatus, const fugio::MidiEvent &pMidiEvent );

	void updateMidiPin( QSharedPointer<fugio::PinInterface> pPin, fugio::MidiInterface *pVal );

private:
	QSharedPointer<fugio::PinInterface>		 mPinInputMidi;
	fugio::MidiInputInterface				*mValInputMidi;

	typedef QPair<QSharedPointer<fugio::PinInterface>,fugio::MidiInterface *>	MidiPinPair;

	QVector<MidiPinPair>					 mOutputChannels;

	QSharedPointer<fugio::PinInterface>		 mPinOutputSystem;
	fugio::MidiInterface					*mValOutputSystem;

	QSharedPointer<fugio::PinInterface>		 mPinOutputSysEx;
	fugio::VariantInterface					*mValOutputSysEx;

	bool									 mSysEx;

	QByteArray								 mSysExCurrent;
	QVector<fugio::MidiEvent>				 mSysExEvents;
};

#endif // MIDIDECODERNODE_H
