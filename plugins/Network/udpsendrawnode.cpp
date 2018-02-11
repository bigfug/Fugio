#include "udpsendrawnode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/core/array_interface.h>
#include <fugio/core/variant_interface.h>
#include <fugio/serialise_interface.h>
#include <fugio/performance.h>
#include <fugio/pin_variant_iterator.h>

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

void UDPSendRawNode::inputsUpdated( qint64 pTimeStamp )
{
	fugio::Performance	Perf( mNode, "inputsUpdated", pTimeStamp );

	if( mAddress == QHostAddress::Null || mPinHost->isUpdated( pTimeStamp ) )
	{
		mAddress = QHostAddress::Null;

		QHostInfo::lookupHost( variant( mPinHost ).toString(), this, SLOT(hostLookup(QHostInfo)) );
	}

	if( mAddress == QHostAddress::Null )
	{
		return;
	}

	if( !mPinData->isUpdated( mWriteTime ) )
	{
		return;
	}

	int							Port = variant<int>( mPinPort );

	fugio::PinVariantIterator	Data( mPinData );

	for( int i = 0 ; i < Data.count() ; i++ )
	{
		const QVariant	VarDat = Data.index( i );

		if( VarDat.canConvert( QVariant::ByteArray ) )
		{
			mSocket.writeDatagram( VarDat.toByteArray(), mAddress, Port );
		}
	}

	mWriteTime = pTimeStamp + 1;
}

void UDPSendRawNode::hostLookup( QHostInfo pHostInfo )
{
	if( !pHostInfo.addresses().isEmpty() )
	{
		mAddress = pHostInfo.addresses().first();

		mNode->context()->updateNode( mNode );
	}
}
