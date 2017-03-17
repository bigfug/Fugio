#include "joinnode.h"

#include <fugio/core/uuid.h>
#include <fugio/colour/colour_interface.h>
#include <fugio/core/list_interface.h>

JoinNode::JoinNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_OUTPUT_JOIN, "D726274E-E95E-409A-8B44-603707B7FCF7" );

	mValOutput = pinOutput<fugio::osc::JoinInterface *>( "Join", mPinOutput, PID_OSC_JOIN, PIN_OUTPUT_JOIN );
}

void JoinNode::oscJoin( QStringList pPath, const QVariant &pValue )
{
	mValOutput->oscJoin( pPath, pValue );
}

void JoinNode::oscPath( QStringList &pPath ) const
{
	mValOutput->oscPath( pPath );
}

QList<QUuid> JoinNode::pinAddTypesInput() const
{
	static QList<QUuid> PinLst =
	{
		PID_OSC_JOIN,
		PID_BOOL,
		PID_COLOUR,
		PID_STRING,
		PID_FLOAT,
		PID_INTEGER,
		PID_BYTEARRAY,
		PID_OSC_SPLIT,
		PID_LIST
	};

	return( PinLst );
}

bool JoinNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_OUTPUT );
}

void JoinNode::inputsUpdated( qint64 pTimeStamp )
{
	for( QSharedPointer<fugio::PinInterface> P : mNode->enumInputPins() )
	{
		if( !P->isUpdated( pTimeStamp ) )
		{
			continue;
		}

		if( input<fugio::osc::JoinInterface *>( P ) )
		{
			continue;
		}

		fugio::ListInterface		*L = input<fugio::ListInterface *>( P );

		if( L )
		{
			QVariantList		VL;

			for( int i = 0 ; i < L->listSize() ; i++ )
			{
				VL << L->listIndex( i );
			}

			mValOutput->oscJoin( QStringList( P->name() ), VL );

			continue;
		}

		fugio::ColourInterface		*C = input<fugio::ColourInterface *>( P );

		if( C )
		{
			mValOutput->oscJoin( QStringList( P->name() ), C->colour() );

			continue;
		}

		fugio::VariantInterface	*V = input<fugio::VariantInterface *>( P );

		if( V )
		{
			mValOutput->oscJoin( QStringList( P->name() ), V->variant() );

			continue;
		}

		bool				 B;

		int					 I = P->value().toInt( &B );

		if( !B )
		{
			continue;
		}

		mValOutput->oscJoin( QStringList( P->name() ) , I );
	}
}


QUuid JoinNode::pinAddControlUuid( fugio::PinInterface *pPin ) const
{
	if( pPin->direction() == PIN_OUTPUT )
	{
		return( PID_OSC_JOIN );
	}

	return( QUuid() );
}
