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

DeviceMidi::DeviceMap			 DeviceMidi::mMidiInputDevices;
DeviceMidi::DeviceMap			 DeviceMidi::mMidiOutputDevices;

bool DeviceMidi::deviceInitialise( void )
{
	PmError			Error;

	if( ( Error = Pm_Initialize() ) != pmNoError )
	{
		qWarning() << "PortMidi failed to initialise" << QString( Pm_GetErrorText( Error ) );

		return( false );
	}

	updateDeviceList();

	return( true );
}

void DeviceMidi::updateDeviceList()
{
	for( int i = 0 ; i < Pm_CountDevices() ; i++ )
	{
		const PmDeviceInfo	*DevInf = Pm_GetDeviceInfo( i );

		if( !DevInf )
		{
			continue;
		}

		const QString		 DevNam = nameFromDeviceInfo( DevInf );

		if( DevInf->input && mMidiInputDevices.find( DevNam ) == mMidiInputDevices.end() )
		{
			mMidiInputDevices.insert( DevNam, QWeakPointer<DeviceMidi>() );
		}

		if( DevInf->output && mMidiOutputDevices.find( DevNam ) == mMidiOutputDevices.end() )
		{
			mMidiOutputDevices.insert( DevNam, QWeakPointer<DeviceMidi>() );
		}
	}
}

void DeviceMidi::deviceDeinitialise( void )
{
	mMidiInputDevices.clear();
	mMidiOutputDevices.clear();

	Pm_Terminate();
}

void DeviceMidi::devicePacketStart( void )
{
	for( QSharedPointer<DeviceMidi> DM : mMidiInputDevices.values() )
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
	for( QSharedPointer<DeviceMidi> DM : mMidiOutputDevices.values() )
	{
		if( !DM )
		{
			continue;
		}

		DM->packetEnd();
	}
}

QSharedPointer<DeviceMidi> DeviceMidi::findDeviceInput( const QString &pName )
{
	QSharedPointer<DeviceMidi>		DevPtr;

	DeviceMap::iterator				it = mMidiInputDevices.find( pName );

	if( it == mMidiInputDevices.end() && !pName.startsWith( '<' ) )
	{
		mMidiInputDevices.insert( pName, QWeakPointer<DeviceMidi>() );

		it = mMidiInputDevices.find( pName );
	}

	if( it == mMidiOutputDevices.end() )
	{
		return( DevPtr );
	}

	DevPtr = it.value().toStrongRef();

	if( !DevPtr )
	{
		if( ( DevPtr = QSharedPointer<DeviceMidi>( new DeviceMidi( pName, MidiDirection::INPUT ) ) ) != nullptr )
		{
			it.value() = DevPtr;
		}
	}

	return( DevPtr );
}

QSharedPointer<DeviceMidi> DeviceMidi::findDeviceOutput( const QString &pName )
{
	QSharedPointer<DeviceMidi>		DevPtr;

	DeviceMap::iterator				it = mMidiOutputDevices.find( pName );

	if( it == mMidiOutputDevices.end() && !pName.startsWith( '<' ) )
	{
		mMidiOutputDevices.insert( pName, QWeakPointer<DeviceMidi>() );

		it = mMidiOutputDevices.find( pName );
	}

	if( it == mMidiOutputDevices.end() )
	{
		return( DevPtr );
	}

	DevPtr = it.value().toStrongRef();

	if( !DevPtr )
	{
		if( ( DevPtr = QSharedPointer<DeviceMidi>( new DeviceMidi( pName, MidiDirection::OUTPUT ) ) ) != nullptr )
		{
			it.value() = DevPtr;
		}
	}

	return( DevPtr );
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

//-----------------------------------------------------------------------------
// Class instance

DeviceMidi::DeviceMidi( const QString &pDeviceName, MidiDirection pDirection )
	: mDeviceName( pDeviceName ), mStreamInput( nullptr ), mStreamOutput( nullptr )
{
	PmError		ER;

	if( pDirection == MidiDirection::INPUT )
	{
		mDeviceId = findDeviceInputId( mDeviceName );

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
	else
	{
		mDeviceId = findDeviceOutputId( mDeviceName );

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
