#include "devicemidi.h"

#include <QSettings>
#include <QHash>

#include "porttime.h"

#include <fugio/utils.h>

#include <fugio/context_interface.h>
#include <fugio/node_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>
#include <fugio/core/variant_interface.h>
#include <fugio/core/uuid.h>

#define TIME_PROC ((int32_t (*)(void *)) Pt_Time)
#define TIME_INFO NULL

QList<QWeakPointer<DeviceMidi>>	 DeviceMidi::mDeviceList;

bool DeviceMidi::deviceInitialise( void )
{
#if defined( PORTMIDI_SUPPORTED )
	if( Pt_Start( 1, nullptr, nullptr ) != ptNoError )
	{
		return( false );
	}

	PmError			Error;

	if( ( Error = Pm_Initialize() ) != pmNoError )
	{
		qWarning() << "PortMidi failed to initialise" << QString( Pm_GetErrorText( Error ) );

		return( false );
	}

	PmDeviceID	DevCnt = Pm_CountDevices();

	for( PmDeviceID DevIdx = 0 ; DevIdx < DevCnt ; DevIdx++ )
	{
		const PmDeviceInfo	*DevInf = Pm_GetDeviceInfo( DevIdx );

		qDebug() << DevIdx << QString( DevInf->interf ) << QString( DevInf->name ) << DevInf->input << DevInf->output;
	}

	qDebug() << "Default Input:" << Pm_GetDefaultInputDeviceID() << "Default Output:" << Pm_GetDefaultOutputDeviceID();
#endif

	return( true );
}

QString DeviceMidi::deviceOutputDefaultName()
{
#if defined( PORTMIDI_SUPPORTED )
	return( deviceName( Pm_GetDefaultOutputDeviceID() ) );
#else
	return( QString() );
#endif
}

QString DeviceMidi::deviceInputDefaultName()
{
#if defined( PORTMIDI_SUPPORTED )
	return( deviceName( Pm_GetDefaultInputDeviceID() ) );
#else
	return( QString() );
#endif
}

#if defined( PORTMIDI_SUPPORTED )
QSharedPointer<DeviceMidi> DeviceMidi::newDevice( PmDeviceID pDevIdx )
{
	for( QSharedPointer<DeviceMidi> DevIns : mDeviceList )
	{
		if( DevIns && DevIns->mDeviceId == pDevIdx )
		{
			return( DevIns );
		}
	}

	QSharedPointer<DeviceMidi> NewDev = QSharedPointer<DeviceMidi>( new DeviceMidi( pDevIdx ) );

	if( NewDev )
	{
		mDeviceList.append( NewDev );
	}

	return( NewDev );
}
#endif

void DeviceMidi::deviceDeinitialise( void )
{
	mDeviceList.clear();

#if defined( PORTMIDI_SUPPORTED )
	Pm_Terminate();

	Pt_Stop();
#endif
}

void DeviceMidi::devicePacketStart( void )
{
	for( QSharedPointer<DeviceMidi> DM : mDeviceList )
	{
#if defined( PORTMIDI_SUPPORTED )
		if( !DM || !DM->mStreamInput )
		{
			continue;
		}

		DM->packetStart();
#endif
	}
}

void DeviceMidi::devicePacketEnd()
{
	for( QSharedPointer<DeviceMidi> DM : mDeviceList )
	{
		if( !DM )
		{
			continue;
		}

		DM->packetEnd();
	}
}

QStringList DeviceMidi::deviceInputNames()
{
	QStringList			DevLst;

#if defined( PORTMIDI_SUPPORTED )
	for( int i = 0 ; i < Pm_CountDevices() ; i++ )
	{
		const PmDeviceInfo	*DevInf = Pm_GetDeviceInfo( i );

		if( !DevInf )
		{
			continue;
		}

		if( DevInf->input )
		{
			DevLst << nameFromDeviceInfo( DevInf );
		}
	}
#endif

	return( DevLst );
}

QStringList DeviceMidi::deviceOutputNames()
{
	QStringList			DevLst;

#if defined( PORTMIDI_SUPPORTED )
	for( int i = 0 ; i < Pm_CountDevices() ; i++ )
	{
		const PmDeviceInfo	*DevInf = Pm_GetDeviceInfo( i );

		if( !DevInf )
		{
			continue;
		}

		if( DevInf->output )
		{
			DevLst << nameFromDeviceInfo( DevInf );
		}
	}
#endif

	return( DevLst );
}

#if defined( PORTMIDI_SUPPORTED )
PmDeviceID DeviceMidi::deviceOutputNameIndex(const QString &pDeviceName)
{
	PmDeviceID	DevCnt = Pm_CountDevices();

	for( PmDeviceID DevIdx = 0 ; DevIdx < DevCnt ; DevIdx++ )
	{
		const PmDeviceInfo	*DevInf = Pm_GetDeviceInfo( DevIdx );

		if( DevInf->output <= 0 )
		{
			continue;
		}

		if( pDeviceName == deviceName( DevIdx ) )
		{
			return( DevIdx );
		}
	}

	return( pmNoDevice );
}

PmDeviceID DeviceMidi::deviceInputNameIndex(const QString &pDeviceName)
{
	PmDeviceID	DevCnt = Pm_CountDevices();

	for( PmDeviceID DevIdx = 0 ; DevIdx < DevCnt ; DevIdx++ )
	{
		const PmDeviceInfo	*DevInf = Pm_GetDeviceInfo( DevIdx );

		if( DevInf->input <= 0 )
		{
			continue;
		}

		if( pDeviceName == deviceName( DevIdx ) )
		{
			return( DevIdx );
		}
	}

	return( pmNoDevice );
}

QString DeviceMidi::nameFromDeviceInfo( const PmDeviceInfo *pDevInf )
{
	return( QString( "%1: %2" ).arg( pDevInf->interf ).arg( pDevInf->name ) );
}

PmDeviceID DeviceMidi::findDeviceInputId( const QString &pName )
{
	for( int i = 0 ; i < Pm_CountDevices() ; i++ )
	{
		const PmDeviceInfo	*DevInf = Pm_GetDeviceInfo( i );

		if( !DevInf )
		{
			continue;
		}

		if( !DevInf->input )
		{
			continue;
		}

		if( nameFromDeviceInfo( DevInf ) == pName )
		{
			return( i );
		}
	}

	return( pmNoDevice );
}

PmDeviceID DeviceMidi::findDeviceOutputId( const QString &pName )
{
	for( int i = 0 ; i < Pm_CountDevices() ; i++ )
	{
		const PmDeviceInfo	*DevInf = Pm_GetDeviceInfo( i );

		if( !DevInf )
		{
			continue;
		}

		if( !DevInf->output )
		{
			continue;
		}

		if( nameFromDeviceInfo( DevInf ) == pName )
		{
			return( i );
		}
	}

	return( pmNoDevice );
}


QString DeviceMidi::deviceName( PmDeviceID pDevIdx )
{
	if( pDevIdx == pmNoDevice )
	{
		return( QString() );
	}

	const PmDeviceInfo	*DevInf = Pm_GetDeviceInfo( pDevIdx );

	if( !DevInf )
	{
		return( QString() );
	}

	return( nameFromDeviceInfo( DevInf ) );
}
#endif

//-----------------------------------------------------------------------------
// Class instance

#if defined( PORTMIDI_SUPPORTED )
DeviceMidi::DeviceMidi( PmDeviceID pDevIdx )
	: mDeviceName( deviceName( pDevIdx ) ), mDeviceId( pDevIdx ), mStreamInput( nullptr ), mStreamOutput( nullptr )
{
	PmError		ER;

	const PmDeviceInfo	*DevInf = Pm_GetDeviceInfo( pDevIdx );

	if( !DevInf )
	{
		qWarning() << "PortMidi input device no info:" << mDeviceName;
	}
	else if( DevInf->input )
	{
		if( mDeviceId == pmNoDevice )
		{
			qWarning() << "PortMidi input device not available:" << mDeviceName;
		}
		else if( ( ER = Pm_OpenInput( &mStreamInput, mDeviceId, nullptr, 0, TIME_PROC, TIME_INFO ) ) != pmNoError )
		{
			qWarning() << "PortMidi input device failed to open:" << mDeviceName << QString( Pm_GetErrorText( ER ) );
		}
		else if( mStreamInput )
		{
			Pm_SetFilter( mStreamInput, PM_FILT_ACTIVE );
		}
	}
	else if( DevInf->output )
	{
		if( mDeviceId == pmNoDevice )
		{
			qWarning() << "PortMidi output device not available:" << mDeviceName;
		}
		else if( ( ER = Pm_OpenOutput( &mStreamOutput, mDeviceId, nullptr, 0, TIME_PROC, TIME_INFO, 0 ) ) != pmNoError )
		{
			qWarning() << "PortMidi output device failed to open:" << mDeviceName << QString( Pm_GetErrorText( ER ) );
		}
	}
}
#endif

DeviceMidi::~DeviceMidi( void )
{
#if defined( PORTMIDI_SUPPORTED )
	if( mStreamInput )
	{
		Pm_Close( mStreamInput );

		mStreamInput = nullptr;
	}

	if( mStreamOutput )
	{
		Pm_Close( mStreamOutput );

		mStreamOutput = nullptr;
	}
#endif
}

void DeviceMidi::packetStart()
{
#if defined( PORTMIDI_SUPPORTED )
	const static int	EVT_BUF_CNT = 128;

	PmEvent		EvtBuf[ EVT_BUF_CNT ];
	int			EvtCnt;

	if( mStreamInput )
	{
		mEvents.clear();

		while( ( EvtCnt = Pm_Read( mStreamInput, &EvtBuf[ 0 ], EVT_BUF_CNT ) ) > 0 )
		{
			int			CurCnt = mEvents.size();

			mEvents.resize( CurCnt + EvtCnt );

			memcpy( &mEvents[ CurCnt ], EvtBuf, sizeof( PmEvent ) * EvtCnt );
		}

		if( !mEvents.isEmpty() )
		{
			QMutexLocker		Lock( &mMidiInputNodesMutex );

			for( fugio::MidiInputInterface *MII : mMidiInputNodes )
			{
				MII->midiProcessInput( reinterpret_cast<const fugio::MidiEvent *>( mEvents.constData() ), mEvents.size() );
			}
		}
	}
#endif
}

void DeviceMidi::packetEnd()
{
#if defined( PORTMIDI_SUPPORTED )
	if( mStreamOutput )
	{
		if( !mSysExBuffer.isEmpty() )
		{
			for( QByteArray &A : mSysExBuffer )
			{
				A.prepend( char( 0xf0 ) );

				A.append( char( 0xf7 ) );

				Pm_WriteSysEx( mStreamOutput, 0, reinterpret_cast<unsigned char *>( A.data() ) );
			}
		}

		if( !mEvents.isEmpty() )
		{
			Pm_Write( mStreamOutput, mEvents.data(), mEvents.size() );
		}

		mSysExBuffer.clear();

		mEvents.clear();
	}
#endif
}

#if defined( PORTMIDI_SUPPORTED )
void DeviceMidi::output( PmEvent pEvent )
{
	if( !mStreamOutput )
	{
		return;
	}

	mEvents.append( pEvent );
}

void DeviceMidi::output( const QVector<PmEvent> &pEvents )
{
	if( !mStreamOutput )
	{
		return;
	}

	mEvents << pEvents;
}
#endif

void DeviceMidi::output( const QVector<fugio::MidiEvent> &pEvents )
{
#if defined( PORTMIDI_SUPPORTED )
	if( !mStreamOutput )
	{
		return;
	}

	for( const fugio::MidiEvent &E : pEvents )
	{
		mEvents << PmEvent{ E.message, E.timestamp };
	}
#endif
}

void DeviceMidi::output( const int32_t *pEvents, int pCount )
{
#if defined( PORTMIDI_SUPPORTED )
	if( !mStreamOutput )
	{
		return;
	}

	for( int i = 0 ; i < pCount ; i++ )
	{
		mEvents << PmEvent{ pEvents[ i ], 0 };
	}
#endif
}

void DeviceMidi::outputSysEx( const QByteArray pArray )
{
#if defined( PORTMIDI_SUPPORTED )
	if( !mStreamOutput )
	{
		return;
	}

	mSysExBuffer << pArray;
#endif
}
