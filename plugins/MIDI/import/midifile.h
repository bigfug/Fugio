#ifndef MIDIFILE_H
#define MIDIFILE_H

#include <QByteArray>
#include <QBuffer>

#include <fugio/midi/midi_input_interface.h>

class MidiFile
{
public:
	MidiFile();

	virtual ~MidiFile();

	void parseMidiData( QByteArray pMidDat );

	const QList<fugio::MidiEvent> &events( int pTrackIndex ) const
	{
		return( mEvents[ pTrackIndex ] );
	}

	quint32 duration( int pTrackIndex ) const
	{
		return( mTrackEnd[ pTrackIndex ] );
	}

private:
	bool parseMidiHeader( QByteArray pMidDat );

	bool parseMidiTrack( QByteArray pMidDat, const int pTrackIndex );

	quint32 parseVariableLengthValue( QBuffer &pMidBuf );

private:
	quint16										 mFormat;
	quint16										 mTracks;
	quint16										 mDivision;

	QList<fugio::MidiEvent>						 mEvents[ 16 ];
	quint32										 mTrackEnd[ 16 ];

	typedef struct TrackTempo
	{
		quint32			mDeltaTime;
		quint32			mMsPerQtrNote;
		qint64			mTimeStamp;
	} TrackTempo;

	QList<TrackTempo>							 mTrackTempo[ 16 ];
};

#endif // MIDIFILE_H
