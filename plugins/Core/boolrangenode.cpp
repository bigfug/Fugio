#include "boolrangenode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/core/variant_interface.h>

BoolRangeNode::BoolRangeNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mSelectedBool( -1 )
{
	FUGID( PIN_INPUT_NUMBER,	"9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_TRIGGER,	"1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_OUTPUT_BOOL,		"261cc653-d7fa-4c34-a08b-3603e8ae71d5" );

	mPinInputIndex   = pinInput( "Index", PIN_INPUT_NUMBER );
	mPinInputTrigger = pinInput( "Trigger", PIN_INPUT_TRIGGER );

	mPinInputIndex->registerPinInputType( PID_INTEGER );

	QSharedPointer<fugio::PinInterface>			 PO;

	pinOutput<fugio::PinControlInterface *>( "Bool 0", PO, PID_BOOL, PIN_OUTPUT_BOOL );

	mPinInputIndex->setDescription( tr( "Sets the output pin that has the index == Number to true" ) );

	mPinInputTrigger->setDescription( tr( "Triggering this input moves the current index to the next output pin" ) );

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
		}
	}

	return( true );
}

void BoolRangeNode::inputsUpdated( qint64 pTimeStamp )
{
	fugio::VariantInterface	*VI = 0;

	if( mPinInputIndex->isConnectedToActiveNode() && mPinInputIndex->isUpdated( pTimeStamp ) )
	{
		VI = input<fugio::VariantInterface *>( mPinInputIndex );
	}

	QList< QSharedPointer<fugio::PinInterface> >	OutLst = mNode->enumOutputPins();

	int					 I = mSelectedBool;

	if( VI )
	{
		if( QMetaType::Type( VI->variant().type() ) == QMetaType::Double || QMetaType::Type( VI->variant().type() )  == QMetaType::Float )
		{
			double		FltIdx = VI->variant().toDouble();

			if( FltIdx < 1.0 )
			{
				I = qBound( 0, int( FltIdx * double( OutLst.size() ) ), OutLst.size() );
			}
			else
			{
				I = qBound( 0, VI->variant().toInt(), OutLst.size() );
			}
		}
		else
		{
			I = qBound( 0, VI->variant().toInt(), OutLst.size() );
		}
	}
	else if( mPinInputTrigger->isUpdated( pTimeStamp ) )
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

	if( mSelectedBool >= 0 && mSelectedBool < OutLst.size() )
	{
		QSharedPointer<fugio::PinInterface>		PinOut = OutLst[ mSelectedBool ];

		fugio::VariantInterface	*VO = qobject_cast<fugio::VariantInterface *>( PinOut->control()->qobject() );

		if( VO )
		{
			VO->setVariant( false );

			pinUpdated( PinOut );
		}
	}

	mSelectedBool = I;

	if( mSelectedBool >= 0 && mSelectedBool < OutLst.size() )
	{
		QSharedPointer<fugio::PinInterface>		PinOut = OutLst[ mSelectedBool ];

		if( PinOut && PinOut->hasControl() )
		{
			fugio::VariantInterface	*VO = qobject_cast<fugio::VariantInterface *>( PinOut->control()->qobject() );

			if( VO )
			{
				VO->setVariant( true );

				pinUpdated( PinOut );
			}
		}
	}
}

QList<QUuid> BoolRangeNode::pinAddTypesOutput( void ) const
{
	static QList<QUuid>		PinLst =
	{
		PID_BOOL
	};

	return( PinLst );
}

bool BoolRangeNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_INPUT );
}

QUuid BoolRangeNode::pinAddControlUuid( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_INPUT ? PID_BOOL : QUuid() );
}
