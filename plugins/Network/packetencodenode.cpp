#include "packetencodenode.h"

#include <QtEndian>

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>

#include <fugio/pin_variant_iterator.h>

#include "crc32.h"

PacketEncodeNode::PacketEncodeNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_DATA,	"9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_OUTPUT_PACKET,	"1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );

	mPinInputData = pinInput( "Data", PIN_INPUT_DATA );

	mValOutputPacket = pinOutput<fugio::VariantInterface *>( "Packet", mPinOutputPacket, PID_BYTEARRAY, PIN_OUTPUT_PACKET );
}

void PacketEncodeNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	if( mPinInputData->isUpdated( pTimeStamp ) )
	{
		mValOutputPacket->variantClear();

		fugio::PinVariantIterator	Data( mPinInputData );

		for( int i = 0 ; i < Data.count() ; i++ )
		{
			processVariant( Data.index( i ) );
		}

		if( mValOutputPacket->variantCount() )
		{
			pinUpdated( mPinOutputPacket );
		}
	}
}

void PacketEncodeNode::processVariant( QVariant V )
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

void PacketEncodeNode::processByteArray( QByteArray A )
{
	if( A.isEmpty() )
	{
		return;
	}
	
	PktHdr		PacketHeader;

	if( sizeof( PacketHeader ) + A.size() > 0xffff )
	{
		int		PktOff = 0;
		
		while( PktOff < A.size() )
		{
			int		PktSze = qMin<quint16>( A.size() - PktOff, 0xffff - sizeof( PacketHeader ) );
			
			processByteArray( A.mid( PktOff, PktSze ) );
			
			PktOff += PktSze;
		}
		
		return;
	}
	
	PacketHeader.mType   = 0;
	PacketHeader.mLength = qToLittleEndian<quint16>( A.size() + sizeof( PacketHeader ) );
	PacketHeader.mNumber = 0;
	PacketHeader.mACK    = ~0;
	PacketHeader.mCRC    = 0;
	
	A.prepend( (const char *)&PacketHeader, sizeof( PacketHeader ) );

	quint32		CRC = qToLittleEndian<quint32>( CRC32::crc32( A ) );

	memcpy( A.data() + offsetof( PktHdr, mCRC ), &CRC, sizeof( CRC ) );

	mValOutputPacket->variantAppend( A );
}
