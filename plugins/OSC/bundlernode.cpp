#include "bundlernode.h"

#include <QtEndian>

#include <fugio/core/uuid.h>

#include <fugio/performance.h>

BundlerNode::BundlerNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_DATA,		"8004E327-B9C1-4E2F-94C5-BFE74FB6A7A7" );
	FUGID( PIN_OUTPUT_BUNDLE,	"D726274E-E95E-409A-8B44-603707B7FCF7" );

	mPinInput = pinInput( "Input", PIN_INPUT_DATA );

	mValOutput = pinOutput<fugio::VariantInterface *>( "Bundle", mPinOutput, PID_BYTEARRAY, PIN_OUTPUT_BUNDLE );
}

QList<QUuid> BundlerNode::pinAddTypesInput() const
{
	static QList<QUuid> PinLst =
	{
		PID_BYTEARRAY_LIST
	};

	return( PinLst );
}

bool BundlerNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_OUTPUT );
}

void BundlerNode::addData( QByteArray &pBundle, const QByteArray &pPacket )
{
	quint32			Temp32 = pPacket.size();
	quint8			TmpBuf[ 4 ];

	qToBigEndian( Temp32, TmpBuf );

	pBundle.append( (const char *)&TmpBuf, 4 );		buffer( pBundle );

	pBundle.append( pPacket );
}

void BundlerNode::buffer( QByteArray &pArray )
{
	while( pArray.size() % 4 != 0 )
	{
		pArray.append( char( 0x00 ) );
	}
}

void BundlerNode::inputsUpdated( qint64 pTimeStamp )
{
	fugio::Performance( mNode, __FUNCTION__, pTimeStamp );

	QByteArray		Bundle;

	Bundle.append( "#bundle\0" );		buffer( Bundle );
	Bundle.append( char( 0 ) );			buffer( Bundle );
	Bundle.append( char( 0 ) );			buffer( Bundle );

	const int	EmptyBundleSize = Bundle.size();

	for( QSharedPointer<fugio::PinInterface> P : mNode->enumInputPins() )
	{
		if( !P->isUpdated( pTimeStamp ) )
		{
			continue;
		}

		if( !variant( P ).canConvert( QVariant::List ) )
		{
			if( variant( P ).canConvert( QVariant::ByteArray ) )
			{
				addData( Bundle, variant( P ).toByteArray() );
			}
		}
		else
		{
			const QVariantList		VarLst = variant( P ).toList();

			for( const QVariant &V : VarLst )
			{
				if( !V.canConvert( QVariant::ByteArray ) )
				{
					continue;
				}

				addData( Bundle, V.toByteArray() );
			}
		}
	}

	if( Bundle.size() == EmptyBundleSize )
	{
		return;
	}

	mValOutput->setVariant( Bundle );

	pinUpdated( mPinOutput );
}
