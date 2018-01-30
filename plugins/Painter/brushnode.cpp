#include "brushnode.h"

#include <QBrush>

#include <fugio/core/uuid.h>
#include <fugio/colour/uuid.h>
#include <fugio/painter/uuid.h>

BrushNode::BrushNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_COLOUR, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_OUTPUT_BRUSH, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );

	mPinInputColour = pinInput( "Colour", PIN_INPUT_COLOUR );

	mPinInputColour->registerPinInputType( PID_COLOUR );

	mValOutputBrush = pinOutput<fugio::VariantInterface *>( "Brush", mPinOutputBrush, PID_BRUSH, PIN_OUTPUT_BRUSH );
}

void BrushNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	QBrush		Brush;

	Brush.setColor( variant( mPinInputColour ).value<QColor>() );

	QVariant	Var = Brush;

	if( mValOutputBrush->variant() != Var )
	{
		mValOutputBrush->setVariant( Var );

		pinUpdated( mPinOutputBrush );
	}
}

