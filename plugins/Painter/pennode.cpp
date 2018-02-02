#include "pennode.h"

#include <QPen>

#include <fugio/core/uuid.h>
#include <fugio/colour/uuid.h>
#include <fugio/painter/uuid.h>

PenNode::PenNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_COLOUR, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_OUTPUT_PEN, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );

	mPinInputColour = pinInput( "Colour", PIN_INPUT_COLOUR );

	mPinInputColour->registerPinInputType( PID_COLOUR );

	mValOutputPen = pinOutput<fugio::VariantInterface *>( "Pen", mPinOutputPen, PID_PEN, PIN_OUTPUT_PEN );
}

void PenNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	QPen		Pen;

	Pen.setColor( variant( mPinInputColour ).value<QColor>() );

	QVariant	Var = Pen;

	if( mValOutputPen->variant() != Var )
	{
		mValOutputPen->setVariant( Var );

		pinUpdated( mPinOutputPen );
	}
}

