#include "fontnode.h"

#include <QPushButton>
#include <QFontDialog>
#include <QSettings>

#include <fugio/painter/uuid.h>

FontNode::FontNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_OUTPUT_FONT, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );

	mValOutputFont = pinOutput<fugio::VariantInterface *>( "Font", mPinOutputFont, PID_FONT, PIN_OUTPUT_FONT );
}

QWidget *FontNode::gui()
{
	QPushButton		*GUI = new QPushButton( "Font..." );

	connect( GUI, SIGNAL(clicked(bool)), this, SLOT(buttonClicked(bool)) );

	return( GUI );
}

void FontNode::buttonClicked( bool )
{
	bool		OK;
	QFont		F = mValOutputFont->variant().value<QFont>();

	F = QFontDialog::getFont( &OK, F, nullptr, tr( "Choose font..." ) );

	if( OK )
	{
		mValOutputFont->setVariant( F );

		pinUpdated( mPinOutputFont );
	}
}

void FontNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	pinUpdated( mPinOutputFont );
}


void FontNode::loadSettings(QSettings &pSettings)
{
	mValOutputFont->setVariant( pSettings.value( "font", mValOutputFont->variant().value<QFont>() ).value<QFont>() );
}

void FontNode::saveSettings(QSettings &pSettings) const
{
	pSettings.setValue( "font", mValOutputFont->variant().value<QFont>() );
}
