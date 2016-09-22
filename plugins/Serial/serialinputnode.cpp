#include "serialinputnode.h"

#include <fugio/node_interface.h>
#include <fugio/context_interface.h>
#include <fugio/core/uuid.h>
#include <fugio/context_signals.h>

#include <QComboBox>

SerialInputNode::SerialInputNode( QSharedPointer<fugio::NodeInterface> pNode ) :
	NodeControlBase( pNode ), mDevice( 0 )
{
	mValOutput = pinOutput<fugio::VariantInterface *>( "Output", mPinOutput, PID_BYTEARRAY );
}

bool SerialInputNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	connect( mNode->context()->qobject(), SIGNAL(frameStart()), this, SLOT(onFrameStart()) );

	return( true );
}

bool SerialInputNode::deinitialise()
{
	return( NodeControlBase::deinitialise() );
}

void SerialInputNode::onFrameStart( void )
{
	if( !mDevice || mDevice->buffer().isEmpty() )
	{
		return;
	}

	mValOutput->setVariant( mDevice->buffer() );

	pinUpdated( mPinOutput );
}

void SerialInputNode::deviceSelection( int )
{
	QComboBox		*DevLst = qobject_cast<QComboBox *>( sender() );

	mDeviceUuid = DevLst->currentData().toUuid();

	mDevice = DeviceSerial::findDevice( mDeviceUuid );
}

QWidget *SerialInputNode::gui()
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

void SerialInputNode::loadSettings( QSettings &pSettings )
{
	mDeviceUuid = pSettings.value( "uuid", mDeviceUuid ).toUuid();

	if( mDeviceUuid.isNull() && !DeviceSerial::devices().isEmpty() )
	{
		mDeviceUuid = DeviceSerial::devices().first()->uuid();
	}

	mDevice = DeviceSerial::findDevice( mDeviceUuid );
}

void SerialInputNode::saveSettings( QSettings &pSettings ) const
{
	pSettings.setValue( "uuid", mDeviceUuid );
}
