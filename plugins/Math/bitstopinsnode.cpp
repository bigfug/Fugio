#include "bitstopinsnode.h"

#include <QBitArray>

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>

BitsToPinsNode::BitsToPinsNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_BITS, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );

	mPinInputBits = pinInput( "Bits", PIN_INPUT_BITS );
}

void BitsToPinsNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	QBitArray	A = variant( mPinInputBits ).toBitArray();
	int			C = qMin( A.size(), 32 );

	QList< QSharedPointer<fugio::PinInterface> >	PinLst = mNode->enumOutputPins();

	while( PinLst.size() < C )
	{
		QSharedPointer<fugio::PinInterface>  P;

		pinOutput<fugio::PinInterface *>( QString::number( PinLst.size() ), P, PID_BOOL, QUuid::createUuid() );

		PinLst << P;
	}

	for( int i = 0 ; i < C ; i++ )
	{
		bool	V = A.at( i );

		QSharedPointer<fugio::PinInterface>  P = PinLst.at( i );
		fugio::VariantInterface				*O = qobject_cast<fugio::VariantInterface *>( P->hasControl() ? P->control()->qobject() : Q_NULLPTR );

		if( O && O->variant().toBool() != V )
		{
			O->setVariant( V );

			pinUpdated( P );
		}
	}
}
