#include "switchnode.h"

#include <QPen>

#include <fugio/core/uuid.h>
#include <fugio/colour/uuid.h>
#include <fugio/image/uuid.h>

SwitchNode::SwitchNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_SWITCH, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_PAINTER1, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_PAINTER2, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_OUTPUT_PAINTER, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );

	mPinInputSwitch = pinInput( "Switch", PIN_INPUT_SWITCH );

	mPinInputPainter1 = pinInput( "Painter1", PIN_INPUT_PAINTER1 );

//	mPinInputPainter1->registerPinInputType( PID_PAINTER );

	mPinInputPainter2 = pinInput( "Painter2", PIN_INPUT_PAINTER2 );

//	mPinInputPainter2->registerPinInputType( PID_PAINTER );

	mValOutputPainter = pinOutput<fugio::PainterInterface *>( "Painter", mPinOutputPainter, PID_PAINTER, PIN_OUTPUT_PAINTER );

	mValOutputPainter->setSource( this );
}

void SwitchNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	pinUpdated( mPinOutputPainter );
}

void SwitchNode::paint( QPainter &pPainter, const QRect &pRect )
{
	fugio::PainterInterface	*P1 = input<fugio::PainterInterface *>( mPinInputPainter1 );
	fugio::PainterInterface	*P2 = input<fugio::PainterInterface *>( mPinInputPainter2 );

	if( variant( mPinInputSwitch ).toBool() )
	{
		if( P2 )
		{
			P2->paint( pPainter, pRect );
		}
	}
	else
	{
		if( P1 )
		{
			P1->paint( pPainter, pRect );
		}
	}
}
