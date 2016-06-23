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

void UDPSendRawNode::inputsUpdated( qint64 pTimeStamp )
{
	fugio::Performance	Perf( mNode, "inputsUpdated", pTimeStamp );

	if( !pTimeStamp || mAddress == QHostAddress::Null || mPinHost->isUpdated( pTimeStamp ) )
	{
		mAddress = QHostAddress::Null;

		QHostInfo::lookupHost( variant( mPinHost ).toString(), this, SLOT(hostLookup(QHostInfo)) );
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

	QByteArray				 A;

	QSharedPointer<fugio::PinInterface>		P = mPinData->connectedPin();

	if( P )
	{
		fugio::PinControlInterface		*I = input<fugio::PinControlInterface *>( mPinData );

		if( !I )
		{
			return;
		}

		if( P->controlUuid() == PID_BYTEARRAY_LIST )
		{
			fugio::VariantInterface		*V = qobject_cast<fugio::VariantInterface *>( I->qobject() );

			if( V )
			{
				const QVariantList	VarLst = V->variant().toList();

				for( const QVariant VI : VarLst )
				{
					QByteArray	ByteArray = VI.toByteArray();

					if( !ByteArray.isEmpty() )
					{
						mSocket.writeDatagram( ByteArray, mAddress, variant( mPinPort ).toInt() );
					}
				}
			}
		}
	}
	else
	{
		A = mPinData->value().toByteArray();
	}

	if( !A.isEmpty() )
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
