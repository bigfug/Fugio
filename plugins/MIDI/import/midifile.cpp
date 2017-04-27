#include "midifile.h"

#include <cmath>

#include <QtEndian>
#include <QDebug>

MidiFile::MidiFile()
{

}

MidiFile::~MidiFile()
{

}

void MidiFile::parseMidiData( QByteArray pMidDat )
{
	QBuffer DatBuf( &pMidDat );

	DatBuf.open( QIODevice::ReadOnly );

	for( int i = 0 ; i < 16 ; i++ )
	{
		mEvents[ i ].clear();

		mTrackEnd[ i ] = 0;

		mTrackTempo[ i ].clear();
	}

	int		TrackIndex = 0;

	while( !DatBuf.atEnd() )
	{
		char		ChunkType[ 4 ];
		quint32		ChunkLength;
		QByteArray	ChunkData;

		if( DatBuf.read( ChunkType, sizeof( ChunkType ) ) != sizeof( ChunkType ) )
		{
			break;
		}

		if( DatBuf.pos() == 4 && strncmp( ChunkType, "MThd", 4 ) )
		{
			break;
		}

		if( DatBuf.read( (char *)&ChunkLength, sizeof( ChunkLength ) ) != sizeof( ChunkLength ) )
		{
			break;
		}

		ChunkLength = qFromBigEndian( ChunkLength );

		ChunkData.resize( ChunkLength );

		if( ChunkData.size() != ChunkLength )
		{
			break;
		}

		if( DatBuf.read( ChunkData.data(), ChunkData.size() ) != ChunkData.size() )
		{
			break;
		}

		if( !strncmp( ChunkType, "MTrk", 4 ) )
		{
			if( !parseMidiTrack( ChunkData, TrackIndex ) )
			{
				break;
			}

			TrackIndex++;

			continue;
		}

		if( !strncmp( ChunkType, "MThd", 4 ) )
		{
			if( !parseMidiHeader( ChunkData ) )
			{
				break;
			}

			continue;
		}
	}
}

bool MidiFile::parseMidiHeader( QByteArray pMidDat )
{
	if( pMidDat.size() < 6 )
	{
		return( false );
	}

	mFormat   = quint8( pMidDat.at( 0 ) ) << 8 | quint8( pMidDat.at( 1 ) );
	mTracks   = quint8( pMidDat.at( 2 ) ) << 8 | quint8( pMidDat.at( 3 ) );
	mDivision = quint8( pMidDat.at( 4 ) ) << 8 | quint8( pMidDat.at( 5 ) );

	qDebug() << QString::number( mDivision, 16 ) << QString::number( quint8( pMidDat.at( 4 ) ), 16 ) << QString::number( quint8( pMidDat.at( 5 ) ), 16 );

	if( mFormat > 2 )
	{
		return( false );
	}

	if( mDivision & 0x8000 )
	{
		quint8		DeltaUnitsPerSMPTE = ( mDivision >> 0 ) & 0x007f;
		qint8		FramesPerSecond    = ( mDivision >> 7 ) & 0x00ff;

		qDebug() << DeltaUnitsPerSMPTE << FramesPerSecond;
	}
	else
	{
		quint16		UnitsPerQuarterNode = mDivision;

		qDebug() << UnitsPerQuarterNode;
	}

	return( true );
}

bool MidiFile::parseMidiTrack( QByteArray pMidDat, const int pTrackIndex )
{
	QBuffer DatBuf( &pMidDat );

	DatBuf.open( QIODevice::ReadOnly );

	quint8		RunningStatus = 0;
	quint32		DeltaTime     = 0;
	qint64		TimeStamp     = 0;

	while( !DatBuf.atEnd() )
	{
		quint32		DeltaIncrement = parseVariableLengthValue( DatBuf );

		DeltaTime += DeltaIncrement;

		if( !mTrackTempo[ pTrackIndex ].isEmpty() )
		{
			TrackTempo	TT = mTrackTempo[ pTrackIndex ].last();

			quint64		TmpDlt = DeltaTime - TT.mDeltaTime;

			if( mDivision & 0x8000 )
			{
				quint8		DeltaUnitsPerSMPTE = ( mDivision >> 0 ) & 0x007f;
				qint8		FramesPerSecond    = ( mDivision >> 7 ) & 0x00ff;

				if( DeltaTime > 0 )
				{
					TimeStamp = ( DeltaTime * 50 ) / DeltaUnitsPerSMPTE;
					TimeStamp = TimeStamp / qAbs( FramesPerSecond );
				}
			}
			else
			{
				quint64		DeltaUnitsPerQuarterNode = mDivision;
				quint64		MsPerQtrNote = TT.mMsPerQtrNote;

				TimeStamp = TT.mTimeStamp + qint64( ( ( TmpDlt * MsPerQtrNote ) / DeltaUnitsPerQuarterNode ) / 1000 );
			}
		}

		quint8		EventType;

		if( DatBuf.read( (char *)&EventType, 1 ) != 1 )
		{
			return( false );
		}

		if( EventType == 0xFF )		// Meta Event
		{
			quint8		MetaType;

			if( DatBuf.read( (char *)&MetaType, 1 ) != 1 )
			{
				return( false );
			}

			quint32		MetaLength = parseVariableLengthValue( DatBuf );

			QByteArray	MetaData = DatBuf.read( MetaLength );

			switch( MetaType )
			{
				case 0x00:
					{
						quint16		SequenceNumber = ( MetaData.at( 1 ) << 8 ) | ( MetaData.at( 2 ) << 0 );

						qDebug() << "Sequence Number" << SequenceNumber;
					}
					break;

				case 0x02:	// Copyright Notice
					{
						qDebug() << MetaData;
					}
					break;

				case 0x03:	// Sequence/Track Name
					{
						qDebug() << MetaData;
					}
					break;

				case 0x2f:	// End of Track
					mTrackEnd[ pTrackIndex ] = TimeStamp;
					break;

				case 0x51:	// Set Tempo (in microseconds per MIDI quarter-note)
					{
						quint8		T1 = quint8( MetaData.at( 0 ) );
						quint8		T2 = quint8( MetaData.at( 1 ) );
						quint8		T3 = quint8( MetaData.at( 2 ) );

						TrackTempo	TT;

						TT.mMsPerQtrNote = ( T1 << 16 ) | ( T2 << 8 ) | ( T3 << 0 );
						TT.mDeltaTime    = DeltaTime;
						TT.mTimeStamp    = TimeStamp;

						if( mFormat < 2 )
						{
							for( int i = 0 ; i < 16 ; i++ )
							{
								mTrackTempo[ i ] << TT;
							}
						}
						else
						{
							mTrackTempo[ pTrackIndex ] << TT;
						}
					}
					break;

				case 0x58:	// Time Signature
					{
						qint8		nn = quint8( MetaData.at( 0 ) );
						quint8		dd = quint8( MetaData.at( 1 ) );
						qint8		cc = quint8( MetaData.at( 2 ) );
						quint8		bb = quint8( MetaData.at( 3 ) );

						qDebug() << "Time Signature" << QString( "%1/%2" ).arg( nn ).arg( std::pow( 2, dd ) ) << cc << bb;
					}
					break;

				case 0x59:	// Key Signature
					{
						qint8		sf = MetaData.at( 0 );
						quint8		mi = quint8( MetaData.at( 1 ) );

						qDebug() << "Key Signature" << sf << mi;
					}
					break;
			}

			RunningStatus = 0;
		}
		else if( EventType >= 0xF0 )
		{
			switch( EventType )
			{
				case 0xF0:		// Sysex Event
					{
						quint32		SysexLength = parseVariableLengthValue( DatBuf );

						QByteArray	SysexData = DatBuf.read( SysexLength );
					}
					break;

				case 0xF7:		// Sysex Event
					{
						quint32		SysexLength = parseVariableLengthValue( DatBuf );

						QByteArray	SysexData = DatBuf.read( SysexLength );
					}
					break;

				default:
					return( false );
			}

			RunningStatus = 0;
		}
		else
		{
			quint8		DataByte1 = 0;
			quint8		DataByte2 = 0;

			if( ( EventType & 0x80 ) == 0 )
			{
				DataByte1 = EventType;

				EventType = RunningStatus;
			}
			else
			{
				if( DatBuf.read( (char *)&DataByte1, 1 ) != 1 )
				{
					return( false );
				}
			}

			quint8		Channel = ( EventType & 0x0f );

			if( EventType < 0xc0 || EventType > 0xdf )
			{
				if( DatBuf.read( (char *)&DataByte2, 1 ) != 1 )
				{
					return( false );
				}
			}

			fugio::MidiEvent		ME;

			ME.message   = Pm_Message( EventType, DataByte1, DataByte2 );
			ME.timestamp = TimeStamp;

			switch( EventType & 0xf0 )
			{
				case 0x80:
					{
//						ME.message = Pm_Message( EventType, DataByte1, DataByte2 );
					}
					break;

				case 0x90:
					{
//						ME.message = Pm_Message( EventType, DataByte1, DataByte2 );

//						qDebug() << Channel << ( EventType & 0xf0 );
					}
					break;
			}

			if( ME.message )
			{
				mEvents[ Channel ] << ME;
			}

			RunningStatus = EventType;
		}
	}

	return( true );
}

quint32 MidiFile::parseVariableLengthValue( QBuffer &pMidBuf )
{
	quint32		Value = 0;

	while( !pMidBuf.atEnd() )
	{
		quint8		LengthByte;

		if( pMidBuf.read( (char *)&LengthByte, 1 ) != 1 )
		{
			return( false );
		}

		Value <<= 7;
		Value  |= ( LengthByte & 0x7f );

		if( ( LengthByte & 0x80 ) == 0 )
		{
			break;
		}
	}

	return( Value );
}

