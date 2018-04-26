#include "clearnode.h"

#include <QPainter>

#include <fugio/image/uuid.h>
#include <fugio/core/uuid.h>
#include <fugio/painter/uuid.h>
#include <fugio/colour/uuid.h>
#include <fugio/colour/colour_interface.h>

#include <fugio/pin_variant_iterator.h>

ClearNode::ClearNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_COLOUR, "D71ABA83-4DDB-4461-8E5F-3D03B93DABD0" );
	FUGID( PIN_OUTPUT_PAINTER, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );

	mPinInputColour = pinInput( tr( "Colour" ), PIN_INPUT_COLOUR );

	mPinInputColour->registerPinInputType( PID_COLOUR );

	mPinInputColour->setValue( QColor( Qt::black ) );

	mValOutputPainter = pinOutput<fugio::PainterInterface *>( "Painter", mPinOutputPainter, PID_PAINTER, PIN_OUTPUT_PAINTER );

	mValOutputPainter->setSource( this );
}

void ClearNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	pinUpdated( mPinOutputPainter );
}

void ClearNode::paint( QPainter &pPainter, const QRect &pRect )
{
	pPainter.fillRect( pRect, variant( mPinInputColour ).value<QColor>() );
}
