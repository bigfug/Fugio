#include "colourbuttonnode.h"

#include <QDateTime>
#include <QSettings>

#include "fugio/global_interface.h"
#include "fugio/context_interface.h"

#include <fugio/core/uuid.h>
#include <fugio/colour/uuid.h>

#include "colourbutton.h"

ColourButtonNode::ColourButtonNode( QSharedPointer<fugio::NodeInterface> pNode ) :
	NodeControlBase( pNode )
{
	mColour = pinOutput<fugio::ColourInterface *>( "Colour", mPinValue, PID_COLOUR );

	mPinValue->setDescription( tr( "The selected colour" ) );
}

ColourButtonNode::~ColourButtonNode()
{
}

QWidget *ColourButtonNode::gui()
{
	ColourButton	*GUI = new ColourButton();

	GUI->setColour( mColour->colour() );

	connect( GUI, SIGNAL(colourChanged(QColor)), this, SLOT(valueChanged(QColor)) );

	connect( this, SIGNAL(valueUpdated(QColor)), GUI, SLOT(setColour(QColor)) );

	return( GUI );
}

void ColourButtonNode::loadSettings( QSettings &pSettings )
{
	valueChanged( QColor( pSettings.value( "Colour", mColour->colour().name() ).toString() ) );
}

void ColourButtonNode::saveSettings( QSettings &pSettings ) const
{
	pSettings.setValue( "Colour", mColour->colour().name() );
}

void ColourButtonNode::valueChanged( QColor pValue )
{
	if( pValue == mColour->colour() )
	{
		return;
	}

	mColour->setColour( pValue );

	mNode->context()->pinUpdated( mPinValue );

	emit valueUpdated( pValue );
}
