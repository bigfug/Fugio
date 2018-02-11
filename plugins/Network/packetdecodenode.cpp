#include "packetdecodenode.h"

#include <QtEndian>

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>

#include <fugio/pin_variant_iterator.h>

#include "crc32.h"

PacketDecodeNode::PacketDecodeNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_PACKETS, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_OUTPUT_DATA,	"1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );

	mPinInputPackets = pinInput( "Packets", PIN_INPUT_PACKETS );

	mValOutputData = pinOutput<fugio::VariantInterface *>( "Data", mPinOutputData, PID_BYTEARRAY, PIN_OUTPUT_DATA );
}

void PacketDecodeNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	mValOutputData->variantClear();

	fugio::PinVariantIterator	Packets( mPinInputPackets );

	for( int i = 0 ; i < Packets.count() ; i++ )
	{
		processVariant( Packets.index( i ) );
	}

	if( mValOutputData->variantCount() )
	{
		pinUpdated( mPinOutputData );
	}
}

void PacketDecodeNode::processVariant( QVariant V )
{
	if( V.type() == QVariant::List )
	{
		QVariantList	L = V.toList();

		for( QVariant I : L )
		{
			processVariant( I );
		}
	}
	else if( V.canConvert( QVariant::ByteArray ) )
	{
		processByteArray( V.toByteArray() );
	}
}

void PacketDecodeNode::processByteArray( QByteArray A )
{
	if( A.size() < sizeof( PktHdr ) )
	{
		return;
	}

	PktHdr		PacketHeader;

	memcpy( &PacketHeader, A.constData(), sizeof( PacketHeader ) );

	PacketHeader.mLength = qFromLittleEndian( PacketHeader.mLength );

	if( PacketHeader.mLength != A.size() )
	{
		return;
	}

	PacketHeader.mCRC = qFromLittleEndian( PacketHeader.mCRC );

	memset( A.data() + offsetof( PktHdr, mCRC ), 0, sizeof( quint32 ) );

	quint32		CRC = CRC32::crc32( A );

	if( CRC != PacketHeader.mCRC )
	{
		return;
	}

	if( PacketHeader.mLength > sizeof( PacketHeader ) )
	{
		mValOutputData->variantAppend( A.mid( sizeof( PacketHeader ) ) );
	}
}
