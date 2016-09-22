#include "serialoutputnode.h"

#include <fugio/node_interface.h>
#include <fugio/context_interface.h>
#include <fugio/core/uuid.h>
#include <fugio/context_signals.h>
#include <fugio/core/array_interface.h>

#include <QComboBox>

SerialOutputNode::SerialOutputNode( QSharedPointer<fugio::NodeInterface> pNode ) :
	NodeControlBase( pNode ), mDevice( 0 ), mLastSend( 0 )
{
	mPinInput = pinInput( "Input" );
}

void SerialOutputNode::inputsUpdated( qint64 pTimeStamp )
{
	if( !mDevice )
	{
		return;
	}

	NodeControlBase::inputsUpdated( pTimeStamp );

	QByteArray		SerDat;

	fugio::ArrayInterface		*A = input<fugio::ArrayInterface *>( mPinInput );

	if( A )
	{
		SerDat.append( (const char *)A->array(), A->stride() * A->count() );
	}

	fugio::VariantInterface	*V = input<fugio::VariantInterface *>( mPinInput );

	if( V )
	{
		SerDat = V->variant().toByteArray();
	}

	if( !SerDat.isEmpty() )
	{
		mDevice->append( SerDat );
	}
}

void SerialOutputNode::deviceSelection( int )
{
	QComboBox		*DevLst = qobject_cast<QComboBox *>( sender() );

	mDeviceUuid = DevLst->currentData().toUuid();

	mDevice = DeviceSerial::findDevice( mDeviceUuid );
}

QWidget *SerialOutputNode::gui()
{
	QComboBox				*DevLst = new QComboBox();

	if( !DevLst )
	{
		return( 0 );
	}

	for( DeviceSerial *DevSer : DeviceSerial::devices() )
	{
		DevLst->addItem( DevSer->name(), DevSer->uuid() );
	}

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

	mDevice = DeviceSerial::findDevice( mDeviceUuid );
}

void SerialOutputNode::saveSettings( QSettings &pSettings ) const
{
	pSettings.setValue( "uuid", mDeviceUuid );
}
