#include "deviceserial.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QStringList>
#include <QSettings>
#include <QDateTime>

QList<DeviceSerial *>		 DeviceSerial::mDeviceList;

void DeviceSerial::deviceInitialise( void )
{

}

void DeviceSerial::deviceDeinitialise( void )
{
	DeviceSerial		*DevSer;

	while( !mDeviceList.isEmpty() )
	{
		DevSer = mDeviceList.takeFirst();

		delDevice( DevSer );
	}
}

void DeviceSerial::devicePacketStart( qint64 pTimeStamp )
{
	for( DeviceSerial *DevCur : mDeviceList )
	{
		DevCur->packetStart( pTimeStamp );
	}
}

void DeviceSerial::devicePacketEnd()
{
	for( DeviceSerial *DevCur : mDeviceList )
	{
		DevCur->packetEnd();
	}
}

void DeviceSerial::deviceCfgSave( QSettings &pDataStream )
{
	pDataStream.beginGroup( "serial" );

	pDataStream.beginWriteArray( "devices", mDeviceList.size() );

	for( int i = 0 ; i < mDeviceList.size() ; i++ )
	{
		pDataStream.setArrayIndex( i );

		DeviceSerial *DevCur = mDeviceList[ i ];

		DevCur->cfgSave( pDataStream );

		pDataStream.setValue( "name", DevCur->name() );

		pDataStream.setValue( "uuid", DevCur->uuid() );

		pDataStream.setValue( "enabled", DevCur->isEnabled() );
	}

	pDataStream.endArray();

	pDataStream.endGroup();
}

void DeviceSerial::deviceCfgLoad( QSettings &pDataStream )
{
	DeviceSerial		*DevCur;

	while( !mDeviceList.isEmpty() )
	{
		DevCur = mDeviceList.takeFirst();

		delDevice( DevCur );
	}

	pDataStream.beginGroup( "serial" );

	int		DevCnt = pDataStream.beginReadArray( "devices" );

	for( int i = 0 ; i < DevCnt ; i++ )
	{
		pDataStream.setArrayIndex( i );

		if( ( DevCur = newDevice() ) == 0 )
		{
			continue;
		}

		DevCur->setName( pDataStream.value( "name", DevCur->name() ).toString() );

		DevCur->setUuid( pDataStream.value( "uuid", DevCur->uuid() ).value<QUuid>() );

		DevCur->cfgLoad( pDataStream );

		if( pDataStream.value( "enabled", false ).toBool() )
		{
			DevCur->setEnabled( true );
		}
	}

	pDataStream.endArray();

	pDataStream.endGroup();
}

QStringList DeviceSerial::portlist( void )
{
	QStringList					PrtLst;

	for( const QSerialPortInfo &PI : QSerialPortInfo::availablePorts() )
	{
		PrtLst.append( PI.portName() );
	}

	return( PrtLst );
}

QList<qint32> DeviceSerial::baudrates( void )
{
	return( QSerialPortInfo::standardBaudRates() );
}

DeviceSerial *DeviceSerial::newDevice( void )
{
	DeviceSerial		*NewDev = new DeviceSerial();

	if( NewDev )
	{
		mDeviceList.append( NewDev );
	}

	return( NewDev );
}

void DeviceSerial::delDevice( DeviceSerial *pDelDev )
{
	if( pDelDev )
	{
		mDeviceList.removeAll( pDelDev );

		pDelDev->deleteLater();
	}
}

DeviceSerial *DeviceSerial::findDevice(const QUuid &pUuid)
{
	for( DeviceSerial *DevCur : mDeviceList )
	{
		if( DevCur->uuid() == pUuid )
		{
			return( DevCur );
		}
	}

	return( 0 );
}

DeviceSerial::DeviceSerial( QObject *pParent ) :
	QObject( pParent ), mDeviceUuid( QUuid::createUuid() ), mBaudRate( QSerialPort::Baud9600 ), mDataBits( QSerialPort::Data8 ),
	mStopBits( QSerialPort::OneStop ), mParity( QSerialPort::NoParity ), mFlowControl( QSerialPort::NoFlowControl ),
	mSerialPort( 0 )
{
	mLastInit = 0;
}

void DeviceSerial::portOpen( void )
{
	portClose();

	if( mPortName.isEmpty() )
	{
		return;
	}

	if( !QSerialPortInfo( mPortName ).isValid() )
	{
		return;
	}

	mSerialPort.setPortName( mPortName );

	if( !mSerialPort.open( QIODevice::ReadWrite ) )
	{
		return;
	}

	mSerialPort.setBaudRate( mBaudRate );
	mSerialPort.setDataBits( mDataBits );
	mSerialPort.setFlowControl( mFlowControl );
	mSerialPort.setParity( mParity );
	mSerialPort.setStopBits( mStopBits );
	//mSerialPort.setBreakEnabled( true );
}

void DeviceSerial::portClose( void )
{
	if( !mSerialPort.isOpen() )
	{
		return;
	}

	mSerialPort.close();
}

void DeviceSerial::packetStart( qreal pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	if( !mSerialPort.isOpen() || !mSerialPort.isReadable() )
	{
		mLastInit = 0;

		return;
	}

	mBufferInput = mSerialPort.readAll();
}

void DeviceSerial::packetEnd( void )
{
	if( mBufferOutput.isEmpty() )
	{
		return;
	}

	if( mSerialPort.isOpen() && mSerialPort.isWritable() )
	{
		mSerialPort.write( mBufferOutput );
	}

	mBufferOutput.clear();
}

void DeviceSerial::cfgSave( QSettings &pDataStream ) const
{
	pDataStream.setValue( "portname", mPortName );
	pDataStream.setValue( "baudrate", mBaudRate );
	pDataStream.setValue( "databits", int( mDataBits ) );
	pDataStream.setValue( "stopbits", int( mStopBits ) );
	pDataStream.setValue( "parity", int( mParity ) );
	pDataStream.setValue( "flowcontrol", int( mFlowControl ) );
}

void DeviceSerial::cfgLoad( QSettings &pDataStream )
{
	mPortName = pDataStream.value( "portname", mPortName ).toString();
	mBaudRate = pDataStream.value( "baudrate", mBaudRate ).toInt();

	mDataBits    = static_cast<QSerialPort::DataBits>( pDataStream.value( "databits", int( mDataBits ) ).toInt() );
	mStopBits    = static_cast<QSerialPort::StopBits>( pDataStream.value( "stopbits", int( mStopBits ) ).toInt() );
	mParity      = static_cast<QSerialPort::Parity>( pDataStream.value( "parity", int( mParity ) ).toInt() );
	mFlowControl = static_cast<QSerialPort::FlowControl>( pDataStream.value( "flowcontrol", int( mFlowControl ) ).toInt() );
}

bool DeviceSerial::isEnabled() const
{
	return( mSerialPort.isOpen() );
}

void DeviceSerial::setEnabled( bool pEnabled )
{
	if( pEnabled )
	{
		portOpen();
	}
	else
	{
		portClose();
	}
}

void DeviceSerial::append( const QByteArray &pOutputBuffer )
{
	mBufferOutput.append( pOutputBuffer );
}

void DeviceSerial::setPortName( const QString &pPortName )
{
	if( mPortName == pPortName )
	{
		return;
	}

	mPortName = pPortName;
}

void DeviceSerial::setBaudRate( qint32 pBaudRate )
{
	if( mBaudRate == pBaudRate )
	{
		return;
	}

	mBaudRate = pBaudRate;
}

void DeviceSerial::setName( const QString &pName )
{
	if( mDeviceName == pName )
	{
		return;
	}

	mDeviceName = pName;
}

void DeviceSerial::setDataBits( QSerialPort::DataBits pDataBits )
{
	if( mDataBits == pDataBits )
	{
		return;
	}

	mDataBits = pDataBits;
}

void DeviceSerial::setStopBits( QSerialPort::StopBits pStopBits )
{
	if( mStopBits == pStopBits )
	{
		return;
	}

	mStopBits = pStopBits;
}

void DeviceSerial::setParity( QSerialPort::Parity pParity )
{
	if( mParity == pParity )
	{
		return;
	}

	mParity = pParity;
}

void DeviceSerial::setFlowControl( QSerialPort::FlowControl pFlowControl )
{
	if( mFlowControl == pFlowControl )
	{
		return;
	}

	mFlowControl = pFlowControl;
}
