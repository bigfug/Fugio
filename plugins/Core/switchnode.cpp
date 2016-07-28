#include "switchnode.h"

#include <QSettings>

#include "fugio/context_interface.h"
#include "fugio/global_interface.h"

SwitchNode::SwitchNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mCurrentIndex( -1 )
{
	mPinInputValue = pinInput( "InputValue" );

	mPinInputValue->setValue( 0.0 );

	pinInput( "Input1" );
	pinInput( "Input2" );

	if( ( mPinOutputValue = mNode->createPin( "OutputValue", PIN_OUTPUT, next_uuid() ) ) != 0 )
	{
		mNode->addPin( mPinOutputValue );
	}
}

SwitchNode::~SwitchNode( void )
{
}

void SwitchNode::inputsUpdated( qint64 pTimeStamp )
{
	bool		IOK = false;
	double		IDX;

	NodeControlBase::inputsUpdated( pTimeStamp );

	if( mPinInputValue->isConnected() )
	{
		QSharedPointer<fugio::PinControlInterface>		 CTL = mPinInputValue->connectedPin()->control();

		fugio::VariantInterface						*VAR;

		if( ( VAR = qobject_cast<fugio::VariantInterface *>( CTL->qobject() ) ) != 0 )
		{
			IDX = VAR->variant().toDouble( &IOK );
		}
	}
	else
	{
		IDX = mPinInputValue->value().toDouble( &IOK );
	}

	if( !IOK )
	{
		return;
	}

	QList< QSharedPointer<fugio::PinInterface> >	PINS = mNode->enumInputPins();

	PINS.removeAll( mPinInputValue );

	if( PINS.isEmpty() )
	{
		return;
	}

	int		CIDX = qMin( PINS.size() - 1, int( double( PINS.size() ) * IDX ) );

	if( mCurrentIndex != CIDX )
	{
		if( PINS[ CIDX ]->isConnected() )
		{
			mPinOutputValue->setControl( PINS[ CIDX ]->connectedPin()->control() );
		}

		mCurrentIndex = CIDX;
	}

	pinUpdated( mPinOutputValue );
}


bool SwitchNode::canAcceptPin(fugio::PinInterface *pPin) const
{
	if( pPin->direction() == PIN_INPUT )
	{
		return( false );
	}

	return( true );
}
