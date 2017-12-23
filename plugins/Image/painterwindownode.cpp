#include "painterwindownode.h"

PainterWindowNode::PainterWindowNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mPainterWindow( nullptr )
{
	FUGID( PIN_INPUT_IMAGE, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_GEOMETRY, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );

	mPinInputImage = pinInput( "Input", PIN_INPUT_IMAGE );

	mPinInputGeometry = pinInput( "Geometry", PIN_INPUT_GEOMETRY );
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
		if( mPinInputGeometry->isUpdated( pTimeStamp ) )
		{
			QRect		R = variant( mPinInputGeometry ).toRect();

			if( R.isValid() )
			{
				mPainterWindow->setGeometry( R );
			}
		}

		if( mPinInputImage->isUpdated( pTimeStamp ) )
		{
			mPainterWindow->renderPin( mPinInputImage );
		}
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

	connect( mPainterWindow, &PainterWindow::windowUpdated, [=]( void )
	{
		mPainterWindow->renderPin( mPinInputImage );
	} );

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
