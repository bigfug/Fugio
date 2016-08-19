#include "mousenode.h"

#include <fugio/core/uuid.h>
#include <fugio/context_signals.h>

MouseNode::MouseNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_EVENTS, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_OUTPUT_POSITION, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_OUTPUT_LEFT_BUTTON, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );
	//FUGID( PIN_XXX_XXX, "249f2932-f483-422f-b811-ab679f006381" );

	mPinInputEvents = pinInput( "Events", PIN_INPUT_EVENTS );

	mValOutputPosition = pinOutput<fugio::VariantInterface *>( "Position", mPinOutputPosition, PID_POINT, PIN_OUTPUT_POSITION );

	mValOutputLeftButton = pinOutput<fugio::VariantInterface *>( "Left Button", mPinOutputLeftButton, PID_BOOL, PIN_OUTPUT_LEFT_BUTTON );
}

bool MouseNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	connect( mNode->context()->qobject(), SIGNAL(frameStart()), this, SLOT(contextFrameStart()) );

	return( true );
}

bool MouseNode::deinitialise()
{
	if( mNode->context() )
	{
		mNode->context()->qobject()->disconnect( this );
	}

	return( NodeControlBase::deinitialise() );
}

void MouseNode::contextFrameStart()
{
	fugio::InputEventsInterface		*InputEvents = input<fugio::InputEventsInterface *>( mPinInputEvents );

	if( !InputEvents )
	{
		return;
	}

	const QList<QMouseEvent>	&MouseEvents = InputEvents->mouseEvents();

	if( MouseEvents.isEmpty() )
	{
		return;
	}

	QPointF		MousePosition = MouseEvents.last().localPos();

	if( MousePosition != mValOutputPosition->variant().toPointF() )
	{
		mValOutputPosition->setVariant( MousePosition );

		pinUpdated( mPinOutputPosition );
	}

	bool		MouseLeftButton = mValOutputLeftButton->variant().toBool();

	for( const QMouseEvent &MouseEvent : MouseEvents )
	{
		if( MouseEvent.type() == QMouseEvent::MouseButtonPress )
		{
			if( MouseEvent.button() == Qt::LeftButton )
			{
				MouseLeftButton = true;
			}

			continue;
		}

		if( MouseEvent.type() == QMouseEvent::MouseButtonRelease )
		{
			if( MouseEvent.button() == Qt::LeftButton )
			{
				MouseLeftButton = false;
			}

			continue;
		}
	}

	if( MouseLeftButton != mValOutputLeftButton->variant().toBool() )
	{
		mValOutputLeftButton->setVariant( MouseLeftButton );

		pinUpdated( mPinOutputLeftButton );
	}
}
