#include "serialinputnode.h"

#include <fugio/node_interface.h>
#include <fugio/context_interface.h>
#include <fugio/core/uuid.h>
#include <fugio/context_signals.h>

#include <QComboBox>

SerialInputNode::SerialInputNode( QSharedPointer<fugio::NodeInterface> pNode ) :
	NodeControlBase( pNode ), mDevice( 0 )
{
	FUGID( PIN_OUTPUT_DATA, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );

	mValOutput = pinOutput<fugio::VariantInterface *>( "Output", mPinOutput, PID_BYTEARRAY, PIN_OUTPUT_DATA );
}

bool SerialInputNode::deinitialise()
{
	disconnect( mNode->context()->qobject(), SIGNAL(frameStart()), this, SLOT(onFrameStart()) );

	return( NodeControlBase::deinitialise() );
}

void SerialInputNode::onFrameStart( void )
{
	if( !mDevice )
	{
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

	if( mDevice->buffer().isEmpty() )
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

	setDevice( mDeviceUuid );
}

void SerialInputNode::setDevice( const QUuid &pDevUid )
{
	if( mDevice && pDevUid == mDevice->uuid() )
	{
		return;
	}

	mDevice = DeviceSerial::findDevice( mDeviceUuid );

	if( mDevice )
	{
		connect( mNode->context()->qobject(), SIGNAL(frameStart()), this, SLOT(onFrameStart()) );

		mNode->setStatus( fugio::NodeInterface::Initialised );
	}
	else
	{
		disconnect( mNode->context()->qobject(), SIGNAL(frameStart()), this, SLOT(onFrameStart()) );

		mNode->setStatus( fugio::NodeInterface::Error );
	}
}

QWidget *SerialInputNode::gui()
{
	QComboBox				*DevLst = new QComboBox();

	if( !DevLst )
	{
		return( 0 );
	}

	setDevice( mDeviceUuid );	// to update node status

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

void SerialInputNode::loadSettings( QSettings &pSettings )
{
	mDeviceUuid = pSettings.value( "uuid", mDeviceUuid ).toUuid();

	if( mDeviceUuid.isNull() && !DeviceSerial::devices().isEmpty() )
	{
		mDeviceUuid = DeviceSerial::devices().first()->uuid();
	}

	setDevice( mDeviceUuid );
}

void SerialInputNode::saveSettings( QSettings &pSettings ) const
{
	pSettings.setValue( "uuid", mDeviceUuid );
}
