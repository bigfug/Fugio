#include "colourbutton.h"

#include <QColorDialog>
#include <QDebug>

ColourButton::ColourButton( QWidget *pParent ) :
	QPushButton( pParent )
{
	connect( this, SIGNAL(clicked()), this, SLOT(buttonClicked()) );
}

void ColourButton::buttonClicked( void )
{
	QColor		C = QColorDialog::getColor( mColour, 0, QString(), QColorDialog::ShowAlphaChannel );

	if( C.isValid() )
	{
		setColour( C );
	}
}

void ColourButton::setColour( const QColor &pColour )
{
	if( pColour == mColour )
	{
		return;
	}

	mColour = pColour;

	setStyleSheet( QString( "background-color: %1;" ).arg( mColour.name()) );

	emit colourChanged( mColour );
}
