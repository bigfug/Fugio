#include "buttonnode.h"

#include <QSettings>
#include <QPushButton>
#include <QDateTime>

#include <fugio/context_interface.h>
#include <fugio/context_signals.h>
#include <fugio/core/uuid.h>
#include <fugio/gui/uuid.h>

ButtonNode::ButtonNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mToggle( false ), mClicked( false ), mPressed( false ), mReleased( false )
{
//	static const QUuid	PII_LABEL( "{eaa00443-372e-4b17-8393-c15beb8a64ab}" );

//	mPinLabel = pinInput( "Label", PII_LABEL );

	mValTrigger = pinOutput<fugio::VariantInterface *>( "Trigger", mPinTrigger, PID_TRIGGER );

	mValBoolean = pinOutput<fugio::VariantInterface *>( "Boolean", mPinBoolean, PID_BOOL );

	mValToggle  = pinOutput<fugio::VariantInterface *>( "Toggle", mPinToggle, PID_BOOL );

//	mPinLabel->setValue( tr( "Button" ) );

	mPinTrigger->setDescription( tr( "When the button is pressed (not released) this trigger will fire" ) );

	mPinBoolean->setDescription( tr( "This pin is true when the button is pressed, false when it isn't" ) );

	mPinToggle->setDescription( tr( "This boolean will switch between true and false every time the button is pressed" ) );
}

ButtonNode::~ButtonNode()
{

}

QWidget *ButtonNode::gui()
{
//	QPushButton	*GUI = new QPushButton( variant( mPinLabel ).toString() );
	QPushButton	*GUI = new QPushButton( tr( "Button" ) );

	connect( GUI, SIGNAL(clicked()), this, SLOT(onClick()) );

	connect( GUI, SIGNAL(pressed()), this, SLOT(onPressed()) );

	connect( GUI, SIGNAL(released()), this, SLOT(onReleased()) );

	return( GUI );
}

bool ButtonNode::initialise()
{
	//mNode->context()->pinUpdated( mPinTrigger );

	//connect( mNode->context()->context(), SIGNAL(frameStart()), this, SLOT(onContextFrameStart()) );

	return( true );
}

void ButtonNode::onClick()
{
	mClicked = true;

	onContextFrameStart();
}

void ButtonNode::onPressed()
{
	mPressed = true;
	mReleased = false;

	onContextFrameStart();
}

void ButtonNode::onReleased()
{
	mReleased = true;
	mPressed = false;

	onContextFrameStart();
}

void ButtonNode::onContextFrameStart()
{
	if( mClicked )
	{
		mToggle = ( mToggle ? false : true );

		mValToggle->setVariant( mToggle );

		pinUpdated( mPinToggle );

		mClicked = false;
	}

	if( mPressed )
	{
		pinUpdated( mPinTrigger );

		mValBoolean->setVariant( true );

		pinUpdated( mPinBoolean );

		mPressed = false;
	}

	if( mReleased )
	{
		mValBoolean->setVariant( false );

		pinUpdated( mPinBoolean );

		mReleased = false;
	}
}
