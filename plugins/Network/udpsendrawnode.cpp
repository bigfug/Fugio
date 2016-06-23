#include "udpsendrawnode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/core/array_interface.h>
#include <fugio/core/variant_interface.h>
#include <fugio/serialise_interface.h>
#include <fugio/performance.h>

#include "networkplugin.h"

UDPSendRawNode::UDPSendRawNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mWriteTime( 0 )
{
	mPinHost = pinInput( "Host" );
	mPinPort = pinInput( "Port" );
	mPinData = pinInput( "Data" );

	mPinHost->registerPinInputType( PID_STRING );
	mPinPort->registerPinInputType( PID_INTEGER );
	mPinData->registerPinInputType( PID_BYTEARRAY );

	mPinHost->setValue( "localhost" );
	mPinPort->setValue( 7878 );
}

bool UDPSendRawNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	return( true );
}

bool UDPSendRawNode::deinitialise()
{
	return( NodeControlBase::deinitialise() );
}

void UDPSendRawNode::inputsUpdated( qint64 pTimeStamp )
{
	fugio::Performance	Perf( mNode, "inputsUpdated", pTimeStamp );

	if( !pTimeStamp || mPinHost->isUpdated( pTimeStamp ) )
	{
		mAddress = QHostAddress::Null;

		QHostInfo::lookupHost(  variant( mPinHost ).toString(), this, SLOT(hostLookup(QHostInfo)) );
	}

	if( mAddress == QHostAddress::Null )
	{
		return;
	}

	sendData( pTimeStamp );
}

void UDPSendRawNode::sendData( qint64 pTimeStamp )
{
	if( !mPinData->isUpdated( mWriteTime ) )
	{
		return;
	}

	QByteArray				 A = variant( mPinData ).toByteArray();

	if( A.isEmpty() )
	{
		mSocket.writeDatagram( A, mAddress, variant( mPinPort ).toInt() );
	}

	mWriteTime = pTimeStamp + 1;
}

void UDPSendRawNode::hostLookup( QHostInfo pHostInfo )
{
	if( !pHostInfo.addresses().isEmpty() )
	{
		mAddress = pHostInfo.addresses().first();
	}
}
