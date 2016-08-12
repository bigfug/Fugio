#include "devicemidi.h"

#include <portmidi.h>

#include <QSettings>
#include <QHash>

#include <fugio/utils.h>

#include <fugio/context_interface.h>
#include <fugio/node_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>
#include <fugio/core/variant_interface.h>
#include <fugio/core/uuid.h>

QList<QWeakPointer<DeviceMidi>>	 DeviceMidi::mDeviceList;

bool DeviceMidi::deviceInitialise( void )
{
	PmError			Error;

	if( ( Error = Pm_Initialize() ) != pmNoError )
	{
		qWarning() << "PortMidi failed to initialise" << QString( Pm_GetErrorText( Error ) );

		return( false );
	}

	return( true );
}

QString DeviceMidi::deviceOutputDefaultName()
{
	return( deviceName( Pm_GetDefaultOutputDeviceID() ) );
}

QString DeviceMidi::deviceInputDefaultName()
{
	return( deviceName( Pm_GetDefaultInputDeviceID() ) );
}

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

void DeviceMidi::deviceDeinitialise( void )
{
	mDeviceList.clear();

	Pm_Terminate();
}

void DeviceMidi::devicePacketStart( void )
{
	for( QSharedPointer<DeviceMidi> DM : mDeviceList )
	{
		if( !DM || !DM->mStreamInput )
		{
			continue;
		}

		DM->packetStart();
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

	return( DevLst );
}

QStringList DeviceMidi::deviceOutputNames()
{
	QStringList			DevLst;

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

	return( DevLst );
}

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

//-----------------------------------------------------------------------------
// Class instance

DeviceMidi::DeviceMidi( PmDeviceID pDevIdx )
	: mDeviceName( deviceName( pDevIdx ) ), mDeviceId( pDevIdx ), mStreamInput( nullptr ), mStreamOutput( nullptr )
{
	PmError		ER;

	const PmDeviceInfo	*DevInf = Pm_GetDeviceInfo( pDevIdx );

	if( DevInf->input )
	{
		if( mDeviceId == pmNoDevice )
		{
			qWarning() << "PortMidi input device not available:" << mDeviceName;
		}
		else if( ( ER = Pm_OpenInput( &mStreamInput, mDeviceId, 0, 0, 0, 0 ) ) != pmNoError )
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
		else if( ( ER = Pm_OpenOutput( &mStreamOutput, mDeviceId, 0, 0, 0, 0, 0 ) ) != pmNoError )
		{
			qWarning() << "PortMidi output device failed to open:" << mDeviceName << QString( Pm_GetErrorText( ER ) );
		}
	}
}

DeviceMidi::~DeviceMidi( void )
{
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
}

void DeviceMidi::packetStart()
{
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
}

void DeviceMidi::packetEnd()
{
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
}

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

void DeviceMidi::output( const QVector<fugio::MidiEvent> &pEvents )
{
	if( !mStreamOutput )
	{
		return;
	}

	for( const fugio::MidiEvent &E : pEvents )
	{
		mEvents << PmEvent{ E.message, E.timestamp };
	}
}

void DeviceMidi::output( const int32_t *pEvents, int pCount )
{
	if( !mStreamOutput )
	{
		return;
	}

	for( int i = 0 ; i < pCount ; i++ )
	{
		mEvents << PmEvent{ pEvents[ i ], 0 };
	}
}

void DeviceMidi::outputSysEx( const QByteArray pArray )
{
	if( !mStreamOutput )
	{
		return;
	}

	mSysExBuffer << pArray;
}
