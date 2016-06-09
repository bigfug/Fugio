#include "boolrangenode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/core/variant_interface.h>

BoolRangeNode::BoolRangeNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mSelectedBool( -1 )
{
	mPinInput   = pinInput( "Number" );
	mPinTrigger = pinInput( "Trigger" );

	QSharedPointer<fugio::PinInterface>			 PO;

	pinOutput<fugio::PinControlInterface *>( "Bool 1", PO, PID_BOOL );

	mPinInput->setDescription( tr( "Sets the output pin that has the index == Number to true" ) );

	mPinTrigger->setDescription( tr( "Triggering this input moves the current index to the next output pin" ) );

	PO->setDescription( tr( "The first boolean pin, its index is 0" ) );
}

bool BoolRangeNode::initialise()
{
	if( !fugio::NodeControlBase::initialise() )
	{
		return( false );
	}

	for( QSharedPointer<fugio::PinInterface> P : mNode->enumOutputPins() )
	{
		fugio::VariantInterface	*V = qobject_cast<fugio::VariantInterface *>( P->control()->qobject() );

		if( V && V->variant().toBool() )
		{
			V->setVariant( false );

			mNode->context()->pinUpdated( P );
		}
	}

	return( true );
}

void BoolRangeNode::inputsUpdated( qint64 pTimeStamp )
{
	fugio::VariantInterface	*VI = 0;

	if( mPinInput->isConnectedToActiveNode() && mPinInput->isUpdated( pTimeStamp ) )
	{
		VI = input<fugio::VariantInterface *>( mPinInput );
	}

	QList< QSharedPointer<fugio::PinInterface> >	OutLst = mNode->enumOutputPins();

	int					 I = mSelectedBool;

	if( VI )
	{
		I = qBound( 0, int( VI->variant().toDouble() * double( OutLst.size() ) ), OutLst.size() );
	}
	else if( mPinTrigger->isUpdated( pTimeStamp ) )
	{
		I = ( I + 1 ) % OutLst.size();
	}
	else if( I < 0 )
	{
		I = 0;
	}

	if( I == mSelectedBool )
	{
		return;
	}

	if( VI )
	{
		qDebug() << "Setting" << I << mSelectedBool;
	}

	if( mSelectedBool >= 0 && mSelectedBool < OutLst.size() )
	{
		fugio::VariantInterface	*VO = qobject_cast<fugio::VariantInterface *>( OutLst.at( mSelectedBool )->control()->qobject() );

		if( VO )
		{
			if( VI )
			{
				qDebug() << "Clearing" << mSelectedBool;
			}

			VO->setVariant( false );

			mNode->context()->pinUpdated( OutLst.at( mSelectedBool ) );
		}
	}

	mSelectedBool = I;

	fugio::VariantInterface	*VO = qobject_cast<fugio::VariantInterface *>( OutLst.at( mSelectedBool )->control()->qobject() );

	if( VO )
	{
		VO->setVariant( true );

		mNode->context()->pinUpdated( OutLst.at( mSelectedBool ) );
	}
}

QList<QUuid> BoolRangeNode::pinAddTypesOutput( void ) const
{
	static QList<QUuid>		PinLst;

	if( PinLst.isEmpty() )
	{
		PinLst << PID_BOOL;
	}

	return( PinLst );
}

bool BoolRangeNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	if( pPin->direction() == PIN_OUTPUT )
	{
		return( false );
	}

	if( pPin->controlUuid() != PID_BOOL )
	{
		return( false );
	}

	return( true );
}
