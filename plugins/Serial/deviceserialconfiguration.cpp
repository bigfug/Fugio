#include "deviceserialconfiguration.h"
#include "ui_deviceserialconfiguration.h"
#include "deviceserial.h"

#include <QListWidgetItem>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QDebug>

DeviceSerialConfiguration::DeviceSerialConfiguration(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DeviceSerialConfiguration), mDevice( nullptr )
{
	ui->setupUi(this);

	for( DeviceSerial *Device : DeviceSerial::devices() )
	{
		QListWidgetItem		*I = new QListWidgetItem( Device->name() );

		I->setData(	Qt::UserRole, Device->uuid() );

		ui->mDeviceList->addItem( I );
	}

	ui->mDeviceList->sortItems();

	for( qint32 Baud : QSerialPortInfo::standardBaudRates() )
	{
		ui->mBaud->addItem( QString::number( Baud ) );
	}

	for( QSerialPortInfo PortInfo : QSerialPortInfo::availablePorts() )
	{
		ui->mPort->addItem( PortInfo.portName() );
	}
}

DeviceSerialConfiguration::~DeviceSerialConfiguration()
{
	delete ui;
}

void DeviceSerialConfiguration::on_mButtonAdd_clicked()
{
	mDevice = DeviceSerial::newDevice();

	if( !mDevice )
	{
		return;
	}

	mDevice->setBaudRate( 9600 );
	mDevice->setPortName( "COM0" );

	QString		DevNam;
	static int	DevIdx = 0;

	while( true )
	{
		DevNam = QString( "Serial-%1" ).arg( DevIdx++ );

		if( ui->mDeviceList->findItems( DevNam, Qt::MatchFixedString | Qt::MatchCaseSensitive ).isEmpty() )
		{
			break;
		}
	}

	mDevice->setName( DevNam );

	mDeviceName = mDevice->uuid();

	QListWidgetItem		*I = new QListWidgetItem( DevNam );

	I->setData(	Qt::UserRole, mDevice->uuid() );

	ui->mDeviceList->addItem( I );

	ui->mDeviceList->sortItems();

	widgetsUpdate();

}

void DeviceSerialConfiguration::widgetsUpdate()
{
	mDevice = findDevice( mDeviceName );

	if( !mDevice )
	{
		ui->mName->clear();
		ui->mPort->clear();
		ui->mBaud->clear();

		ui->mButtonEnable->setChecked( false );
	}
	else
	{
		ui->mName->setText( mDevice->name() );
		ui->mPort->setCurrentText( mDevice->portname() );
		ui->mBaud->setCurrentText( QString::number( mDevice->baudrate() ) );

		switch( mDevice->dataBits() )
		{
			case QSerialPort::Data5:
				ui->mDataBits->setCurrentText( "5" );
				break;

			case QSerialPort::Data6:
				ui->mDataBits->setCurrentText( "6" );
				break;

			case QSerialPort::Data7:
				ui->mDataBits->setCurrentText( "7" );
				break;

			case QSerialPort::Data8:
			case QSerialPort::UnknownDataBits:
				ui->mDataBits->setCurrentText( "8" );
				break;
		}

		switch( mDevice->stopBits() )
		{
			case QSerialPort::UnknownStopBits:
			case QSerialPort::OneStop:
				ui->mStopBits->setCurrentIndex( 0 );
				break;

			case QSerialPort::OneAndHalfStop:
				ui->mStopBits->setCurrentIndex( 1 );
				break;

			case QSerialPort::TwoStop:
				ui->mStopBits->setCurrentIndex( 2 );
				break;
		}

		switch( mDevice->parity() )
		{
			//case QSerialPort::NoParity:
			default:
				ui->mParity->setCurrentIndex( 0 );
				break;

			case QSerialPort::EvenParity:
				ui->mParity->setCurrentIndex( 1 );
				break;

			case QSerialPort::OddParity:
				ui->mParity->setCurrentIndex( 2 );
				break;
		}

		switch( mDevice->flowControl() )
		{
			case QSerialPort::NoFlowControl:
			case QSerialPort::UnknownFlowControl:
				ui->mFlowControl->setCurrentIndex( 0 );
				break;

			case QSerialPort::HardwareControl:
				ui->mFlowControl->setCurrentIndex( 1 );
				break;

			case QSerialPort::SoftwareControl:
				ui->mFlowControl->setCurrentIndex( 2 );
				break;
		}

		ui->mButtonEnable->setEnabled( true );

		ui->mButtonEnable->setChecked( mDevice->isEnabled() );

		if( !mDevice->isEnabled() )
		{
			ui->mName->setEnabled( true );
			ui->mPort->setEnabled( true );
			ui->mBaud->setEnabled( true );
			ui->mDataBits->setEnabled( true );
			ui->mStopBits->setEnabled( true );
			ui->mFlowControl->setEnabled( true );
			ui->mParity->setEnabled( true );

			ui->mButtonRemove->setEnabled( true );
			ui->mButtonEnable->setEnabled( true );
		}
	}

	if( !mDevice || mDevice->isEnabled() )
	{
		ui->mName->setEnabled( false );
		ui->mPort->setEnabled( false );
		ui->mBaud->setEnabled( false );
		ui->mDataBits->setEnabled( false );
		ui->mStopBits->setEnabled( false );
		ui->mFlowControl->setEnabled( false );
		ui->mParity->setEnabled( false );

		ui->mButtonRemove->setEnabled( false );
		ui->mButtonEnable->setEnabled( true );

		ui->mButtonEnable->setChecked( !mDevice ? false : mDevice->isEnabled() );
	}
}

DeviceSerial *DeviceSerialConfiguration::findDevice( QUuid pDeviceId )
{
	QList<DeviceSerial *>	DevLst = DeviceSerial::devices();

	for( int i = 0 ; i < DevLst.size() ; i++ )
	{
		if( DevLst.at( i )->uuid() != pDeviceId )
		{
			continue;
		}

		return( DevLst[ i ] );
	}

	return( 0 );
}

QListWidgetItem *DeviceSerialConfiguration::findItem(const QUuid &pDeviceId)
{
	for( int i = 0 ; i < ui->mDeviceList->count() ; i++ )
	{
		QListWidgetItem	*I = ui->mDeviceList->item( i );

		if( I && I->data( Qt::UserRole ).value<QUuid>() == pDeviceId )
		{
			return( I );
		}
	}

	return( 0 );
}

void DeviceSerialConfiguration::on_mDeviceList_itemActivated(QListWidgetItem *item)
{
	onDeviceSelected( item->data( Qt::UserRole ).value<QUuid>() );
}

void DeviceSerialConfiguration::on_mDeviceList_itemChanged(QListWidgetItem *item)
{
	if( item )
	{
		onDeviceSelected( item->data( Qt::UserRole ).value<QUuid>() );
	}
	else
	{
		onDeviceSelected( QUuid() );
	}
}

void DeviceSerialConfiguration::onDeviceSelected(QUuid pDeviceName)
{
	mDeviceName = pDeviceName;

	widgetsUpdate();
}

void DeviceSerialConfiguration::on_mName_textChanged(const QString &arg1)
{
	if( mDevice )
	{
		mDevice->setName( arg1 );

		if( QListWidgetItem *I = findItem( mDevice->uuid() ) )
		{
			I->setText( arg1 );

			ui->mDeviceList->sortItems();
		}
	}
}

void DeviceSerialConfiguration::on_mPort_currentTextChanged( const QString &arg1 )
{
	if( mDevice )
	{
		mDevice->setPortName( arg1 );
	}
}

void DeviceSerialConfiguration::on_mBaud_currentTextChanged(const QString &arg1)
{
	if( mDevice )
	{
		mDevice->setBaudRate( arg1.toInt() );
	}
}

void DeviceSerialConfiguration::on_mDeviceList_itemClicked(QListWidgetItem *item)
{
	onDeviceSelected( item->data( Qt::UserRole ).value<QUuid>() );
}

void DeviceSerialConfiguration::on_mButtonRemove_clicked()
{
	if( mDevice )
	{
		if( QListWidgetItem *I = findItem( mDevice->uuid() ) )
		{
			delete I;
		}

		DeviceSerial::delDevice( mDevice );

		mDevice = 0;

		mDeviceName = QUuid();

		widgetsUpdate();
	}
}

void DeviceSerialConfiguration::on_mButtonEnable_toggled( bool checked )
{
	if( mDevice )
	{
		mDevice->setEnabled( checked );

		if( checked && !mDevice->isEnabled() )
		{
			QMessageBox::warning( nullptr, tr( "Serial Port Error" ), tr( "Couldn't open serial port %1 with baud %2" ).arg( mDevice->name() ).arg( mDevice->baudrate() ) );
		}

		widgetsUpdate();
	}
}

void DeviceSerialConfiguration::on_mDataBits_currentIndexChanged(int index)
{
	if( !mDevice )
	{
		return;
	}

	switch( index )
	{
		case 0:
			mDevice->setDataBits( QSerialPort::Data7 );
			break;

		default:
			mDevice->setDataBits( QSerialPort::Data8 );
			break;
	}
}

void DeviceSerialConfiguration::on_mStopBits_currentIndexChanged(int index)
{
	if( !mDevice )
	{
		return;
	}

	switch( index )
	{
		case 0:
			mDevice->setStopBits( QSerialPort::OneStop );
			break;

		case 1:
			mDevice->setStopBits( QSerialPort::OneAndHalfStop );
			break;

		case 2:
			mDevice->setStopBits( QSerialPort::TwoStop );
			break;
	}
}

void DeviceSerialConfiguration::on_mParity_currentIndexChanged(int index)
{
	if( !mDevice )
	{
		return;
	}

	switch( index )
	{
		case 0:
			mDevice->setParity( QSerialPort::NoParity );
			break;

		case 1:
			mDevice->setParity( QSerialPort::EvenParity );
			break;

		case 2:
			mDevice->setParity( QSerialPort::OddParity );
			break;
	}
}

void DeviceSerialConfiguration::on_mFlowControl_currentIndexChanged(int index)
{
	if( !mDevice )
	{
		return;
	}

	switch( index )
	{
		case 0:
			mDevice->setFlowControl( QSerialPort::NoFlowControl );
			break;

		case 1:
			mDevice->setFlowControl( QSerialPort::HardwareControl );
			break;

		case 2:
			mDevice->setFlowControl( QSerialPort::SoftwareControl );
			break;
	}
}
