#ifndef MIDIOUTPUTPIN_H
#define MIDIOUTPUTPIN_H

#include <fugio/core/uuid.h>

#include <QObject>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>
#include <fugio/midi/midi_interface.h>

#include <fugio/pincontrolbase.h>

#include <fugio/serialise_interface.h>

class MidiOutputPin : public fugio::PinControlBase, public fugio::MidiInterface, public fugio::SerialiseInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::MidiInterface fugio::SerialiseInterface )

public:
	Q_INVOKABLE explicit MidiOutputPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~MidiOutputPin( void ) {}

	static void registerMetaType( void );

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE
	{
		return( "" );
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "MIDI Output Pin" );
	}

	//-------------------------------------------------------------------------
	// fugio::MidiInterface

	virtual void addMessage( quint32 pMessage ) Q_DECL_OVERRIDE
	{
		fugio::MidiEvent		E;

		E.timestamp = 0;
		E.message   = pMessage;

		mMessages.append( E );
	}

	virtual void addMessages( const QVector<fugio::MidiEvent> &pMessages ) Q_DECL_OVERRIDE;

	virtual void addSysEx( const QByteArray &pSysEx ) Q_DECL_OVERRIDE
	{
		mSysEx.append( pSysEx );
	}

	virtual QVector<fugio::MidiEvent> messages( void ) const Q_DECL_OVERRIDE;

	virtual QByteArray systemExclusive( void ) const Q_DECL_OVERRIDE;

	virtual void clearData( void ) Q_DECL_OVERRIDE;

	virtual bool hasMessages() const Q_DECL_OVERRIDE
	{
		return( !mMessages.isEmpty() );
	}

	virtual bool hasSysEx() const Q_DECL_OVERRIDE
	{
		return( !mSysEx.isEmpty() );
	}

	//-------------------------------------------------------------------------
	// fugio::SerialiseInterface

	virtual void serialise( QDataStream &pDataStream ) const Q_DECL_OVERRIDE;

	virtual void deserialise( QDataStream &pDataStream ) Q_DECL_OVERRIDE;

private:
	QVector<fugio::MidiEvent>		mMessages;
	QByteArray						mSysEx;
};

#endif // MIDIOUTPUTPIN_H
