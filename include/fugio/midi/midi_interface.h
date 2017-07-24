#ifndef MIDI_INTERFACE_H
#define MIDI_INTERFACE_H

#include <QUuid>
#include <QVariant>

#include <fugio/global.h>

FUGIO_NAMESPACE_BEGIN

#define MIDI_CLOCK      0xf8
#define MIDI_ACTIVE     0xfe
#define MIDI_STATUS_MASK 0x80
#define MIDI_SYSEX      0xf0
#define MIDI_EOX        0xf7
#define MIDI_START      0xFA
#define MIDI_STOP       0xFC
#define MIDI_CONTINUE   0xFB
#define MIDI_F9         0xF9
#define MIDI_FD         0xFD
#define MIDI_RESET      0xFF
#define MIDI_NOTE_ON    0x90
#define MIDI_NOTE_OFF   0x80
#define MIDI_CHANNEL_AT 0xD0
#define MIDI_POLY_AT    0xA0
#define MIDI_PROGRAM    0xC0
#define MIDI_CONTROL    0xB0
#define MIDI_PITCHBEND  0xE0
#define MIDI_MTC        0xF1
#define MIDI_SONGPOS    0xF2
#define MIDI_SONGSEL    0xF3
#define MIDI_TUNE       0xF6

typedef qint32 PmTimestamp;
typedef qint32 PmMessage;

typedef struct MidiEvent
{
	PmMessage      message;
	PmTimestamp    timestamp;
} MidiEvent;

#define Pm_Message(status, data1, data2) \
		 ((((data2) << 16) & 0xFF0000) | \
		  (((data1) << 8) & 0xFF00) | \
		  ((status) & 0xFF))
#define Pm_MessageStatus(msg) ((msg) & 0xFF)
#define Pm_MessageData1(msg) (((msg) >> 8) & 0xFF)
#define Pm_MessageData2(msg) (((msg) >> 16) & 0xFF)

class MidiInterface
{
public:
	virtual ~MidiInterface( void ) {}

	virtual void addMessage( quint32 pMessage ) = 0;

	virtual void addMessages( const QVector<MidiEvent> &pMessages ) = 0;

	virtual void addSysEx( const QByteArray &pSysEx ) = 0;

	virtual QVector<MidiEvent> messages( void ) const = 0;

	virtual QByteArray systemExclusive( void ) const = 0;

	virtual void clearData( void ) = 0;

	virtual bool hasMessages( void ) const = 0;

	virtual bool hasSysEx( void ) const = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_METATYPE( fugio::MidiEvent )

Q_DECLARE_INTERFACE( fugio::MidiInterface, "com.bigfug.fugio.midi/1.0" )

#endif // MIDI_INTERFACE_H

