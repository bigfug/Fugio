#include "serialoutputnode.h"

#include <fugio/node_interface.h>
#include <fugio/context_interface.h>
#include <fugio/core/uuid.h>
#include <fugio/context_signals.h>
#include <fugio/core/variant_interface.h>
#include <fugio/pin_variant_iterator.h>

#include <QComboBox>

SerialOutputNode::SerialOutputNode( QSharedPointer<fugio::NodeInterface> pNode ) :
	NodeControlBase( pNode ), mDevice( 0 ), mLastSend( 0 )
{
	FUGID( PIN_INPUT_DATA, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );

	mPinInput = pinInput( "Input", PIN_INPUT_DATA );
}

void SerialOutputNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	if( !mDevice )
	{
		mNode->setStatus( fugio::NodeInterface::Error );

		return;
	}

	if( !mDevice->isEnabled() )
	{
		mNode->setStatus( fugio::NodeInterface::Warning );
	}
	else
	{
		mNode->setStatus( fugio::NodeInterface::Initialised );
	}

	QByteArray		SerDat;

	fugio::PinVariantIterator		Input( mPinInput );

	const int		OutCnt = Input.count();

	if( !OutCnt )
	{
		return;
	}

	for( int i = 0 ; i < OutCnt ; i++ )
	{
		const QVariant	&V = Input.index( i );

		switch( QMetaType::Type( V.type() ) )
		{
			case QMetaType::QByteArray:
				mDevice->append( V.toByteArray() );
				break;

			default:
				break;
		}
	}
}

void SerialOutputNode::setDevice( const QUuid &pDevUid )
{
	if( mDevice && pDevUid == mDevice->uuid() )
	{
		return;
	}

	mDevice = DeviceSerial::findDevice( mDeviceUuid );

	if( mDevice )
	{
		mNode->setStatus( fugio::NodeInterface::Initialised );
	}
	else
	{
		mNode->setStatus( fugio::NodeInterface::Error );
	}
}

void SerialOutputNode::deviceSelection( int )
{
	QComboBox		*DevLst = qobject_cast<QComboBox *>( sender() );

	mDeviceUuid = DevLst->currentData().toUuid();

	setDevice( mDeviceUuid );
}

QWidget *SerialOutputNode::gui()
{
	QComboBox				*DevLst = new QComboBox();

	if( !DevLst )
	{
		return( 0 );
	}

	if( !mDevice )
	{
		mNode->setStatus( fugio::NodeInterface::Error );
	}
	else
	{
		mNode->setStatus( fugio::NodeInterface::Initialised );
	}

	for( DeviceSerial *DevSer : DeviceSerial::devices() )
	{
		DevLst->addItem( DevSer->name(), DevSer->uuid() );
	}

	DevLst->setCurrentIndex( -1 );

	for( int i = 0 ; i < DevLst->count() ; i++ )
	{
		if( DevLst->itemData( i ).toUuid() == mDeviceUuid )
		{
			DevLst->setCurrentIndex( i );

			break;
		}
	}

	connect( DevLst, SIGNAL(currentIndexChanged(int)), this, SLOT(deviceSelection(int)) );

	connect( this, SIGNAL(deviceSelected(int)), DevLst, SLOT(setCurrentIndex(int)) );

	return( DevLst );
}

void SerialOutputNode::loadSettings( QSettings &pSettings )
{
	mDeviceUuid = pSettings.value( "uuid", mDeviceUuid ).toUuid();

	if( mDeviceUuid.isNull() && !DeviceSerial::devices().isEmpty() )
	{
		mDeviceUuid = DeviceSerial::devices().first()->uuid();
	}

	setDevice( mDeviceUuid );
}

void SerialOutputNode::saveSettings( QSettings &pSettings ) const
{
	pSettings.setValue( "uuid", mDeviceUuid );
}

