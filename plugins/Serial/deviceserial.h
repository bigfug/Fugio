#ifndef DEVICESERIAL_H
#define DEVICESERIAL_H

#include <QObject>
#include <QUuid>

#if defined( SERIALPORT_SUPPORTED )
#include <QSerialPort>
#endif

class QSettings;

class DeviceSerial : public QObject
{
	Q_OBJECT

public:
	static void deviceInitialise( void );
	static void deviceDeinitialise( void );
	static void devicePacketStart( qint64 pTimeStamp );
	static void devicePacketEnd( void );

	static void deviceCfgSave( QSettings &pDataStream );
	static void deviceCfgLoad( QSettings &pDataStream );

	static QStringList portlist( void );

	static QList<qint32> baudrates( void );

	static DeviceSerial *newDevice( void );

	static void delDevice( DeviceSerial *pDelDev );

	static const QList<DeviceSerial *> devices( void )
	{
		return( mDeviceList );
	}

	static DeviceSerial *findDevice( const QUuid &pUuid );

private:
	explicit DeviceSerial( QObject *pParent = 0 );

	virtual ~DeviceSerial( void ) {}

public:
	virtual void packetStart( qreal pTimeStamp );
	virtual void packetEnd( void );

	virtual void cfgSave( QSettings &pDataStream ) const;
	virtual void cfgLoad( QSettings &pDataStream );

	virtual bool isEnabled( void ) const;
	virtual void setEnabled( bool pEnabled );

	inline QString portname( void ) const
	{
		return( mPortName );
	}

	inline qint32 baudrate( void ) const
	{
		return( mBaudRate );
	}

	const QByteArray &buffer( void ) const
	{
		return( mBufferInput );
	}

	void append( const QByteArray &pOutputBuffer );

	QUuid uuid( void ) const
	{
		return( mDeviceUuid );
	}

	QString name( void ) const
	{
		return( mDeviceName );
	}

#if defined( SERIALPORT_SUPPORTED )
	const QSerialPort &port( void ) const
	{
		return( mSerialPort );
	}

	QSerialPort &port( void )
	{
		return( mSerialPort );
	}

	inline qint32 dataBits( void ) const
	{
		return( mDataBits );
	}

	inline QSerialPort::StopBits stopBits( void ) const
	{
		return( mStopBits );
	}

	inline QSerialPort::Parity parity( void ) const
	{
		return( mParity );
	}

	inline QSerialPort::FlowControl flowControl( void ) const
	{
		return( mFlowControl );
	}
#endif

signals:

public slots:
	void setPortName( const QString &pPortName );
	void setBaudRate( qint32 pBaudRate );
	void setName( const QString &pName );

#if defined( SERIALPORT_SUPPORTED )
	void setDataBits( QSerialPort::DataBits pDataBits );
	void setStopBits( QSerialPort::StopBits pStopBits );
	void setParity( QSerialPort::Parity pParity );
	void setFlowControl( QSerialPort::FlowControl pFlowControl );
#endif

private slots:
	void portOpen( void );
	void portClose( void );

	void setUuid( const QUuid &pUuid )
	{
		mDeviceUuid = pUuid;
	}

private:
	QUuid						 mDeviceUuid;
	QString						 mDeviceName;
	QString						 mPortName;
	qint32						 mBaudRate;

#if defined( SERIALPORT_SUPPORTED )
	QSerialPort::DataBits		 mDataBits;
	QSerialPort::StopBits		 mStopBits;
	QSerialPort::Parity			 mParity;
	QSerialPort::FlowControl	 mFlowControl;

	QSerialPort					 mSerialPort;
#endif

	QByteArray					 mBufferInput;
	QByteArray					 mBufferOutput;
	QByteArray					 mCommandBuffer;

	qreal						 mLastInit;

	static QList<DeviceSerial *>		 mDeviceList;
};

#endif // DEVICESERIAL_H
