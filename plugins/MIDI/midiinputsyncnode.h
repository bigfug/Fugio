#ifndef MIDIINPUTSYNCNODE_H
#define MIDIINPUTSYNCNODE_H

#include <fugio/nodecontrolbase.h>
#include <fugio/midi/midi_input_interface.h>
#include <fugio/midi/midi_interface.h>
#include <fugio/core/list_interface.h>

class MidiInputSyncNode : public fugio::NodeControlBase, public fugio::MidiInputInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::MidiInputInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Midi_Input_Sync" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE MidiInputSyncNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~MidiInputSyncNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface interface

	//-------------------------------------------------------------------------
	// MidiInputInterface interface

public:
	virtual void midiProcessInput( const fugio::MidiEvent *pMessages, quint32 pMessageCount ) Q_DECL_OVERRIDE;

private:
	static unsigned short CombineBytes( unsigned char First, unsigned char Second )
	{
		unsigned short _14bit;
		_14bit = (unsigned short)Second;
		_14bit <<= 7;
		_14bit |= (unsigned short)First;
		return(_14bit);
	}

	void processMTC( int h, int m, int s, int f, int t, bool pPlay );

	void processMidiClockMessage( const fugio::PmTimestamp EV );

private:
	QSharedPointer<fugio::PinInterface>		 mPinInputMidi;
	fugio::MidiInputInterface				*mValInputMidi;

	QSharedPointer<fugio::PinInterface>		 mPinSync;
	fugio::VariantInterface					*mValSync;

	QSharedPointer<fugio::PinInterface>		 mPinBPM;
	fugio::VariantInterface					*mValBPM;

	bool									 mMidiPlay;
	bool									 mMidiPlayOnNext;

	int										 mfr, msc, mmn, mhr, mst;
	quint8									 mls;
	int										 mbc;
	qreal									 mMidiTime;

	quint16									 mSngPosPtr;
	int										 mMidiClockCount;

	QList<fugio::PmTimestamp>				 mMidiClockEvents;
	qreal									 mMidiClockPeriod;
	qreal									 mMidiClockBPM;
};

#endif // MIDIINPUTSYNCNODE_H
