#include "painterwindownode.h"

PainterWindowNode::PainterWindowNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mPainterWindow( nullptr )
{
	mPinInputImage = pinInput( "Input" );
}

QWidget *PainterWindowNode::gui( void )
{
	return( 0 );
}

void PainterWindowNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	if( mPainterWindow )
	{
		mPainterWindow->renderPin( mPinInputImage );
	}
}

bool PainterWindowNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	if( ( mPainterWindow = new PainterWindow() ) == nullptr )
	{
		return( false );
	}

	mPainterWindow->show();

	return( true );
}

bool PainterWindowNode::deinitialise()
{
	if( mPainterWindow )
	{
		delete mPainterWindow;

		mPainterWindow = nullptr;
	}

	return( NodeControlBase::deinitialise() );
}
