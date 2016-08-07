#include "screennode.h"

#include <QScreen>
#include <QApplication>
#include <QSettings>

#include <fugio/core/uuid.h>

ScreenNode::ScreenNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_SCREEN,	"0d6c033f-3e24-4b13-9fd1-a207b45173ec" );
	FUGID( PIN_OUTPUT_GEOMETRY, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );

	mValInputScreen = pinInput<fugio::ChoiceInterface *>( "Screen", mPinInputScreen, PID_CHOICE, PIN_INPUT_SCREEN );

	mValOutputGeometry = pinOutput<fugio::VariantInterface *>( "Geometry", mPinOutputGeometry, PID_RECT, PIN_OUTPUT_GEOMETRY );

	updateScreenList();
}

bool ScreenNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	QScreen		*Screen = qApp->primaryScreen();

	setScreenName( Screen->name() );

	return( true );
}

void ScreenNode::inputsUpdated( qint64 pTimeStamp )
{
	if( mPinInputScreen->isUpdated( pTimeStamp ) )
	{
		setScreenName( variant( mPinInputScreen ).toString() );
	}
}

void ScreenNode::loadSettings(QSettings &pSettings)
{
	setScreenName( pSettings.value( "screen", mScreenName ).toString() );
}

void ScreenNode::saveSettings(QSettings &pSettings) const
{
	pSettings.setValue( "screen", mScreenName );
}

void ScreenNode::setScreenName( QString pName )
{
	if( pName == mScreenName )
	{
		return;
	}

	QScreen		*Screen = fromName( mScreenName );

	if( Screen )
	{
		Screen->disconnect( this );
	}

	mScreenName = pName;

	if( ( Screen = fromName( mScreenName ) ) != nullptr )
	{
		connect( Screen, SIGNAL(virtualGeometryChanged(QRect)), this, SLOT(screenVirtualGeometryChanged(QRect)) );

		screenVirtualGeometryChanged( Screen->virtualGeometry() );
	}
}

void ScreenNode::screenVirtualGeometryChanged( QRect pRect )
{
	if( mValOutputGeometry->variant().toRect() != pRect )
	{
		mValOutputGeometry->setVariant( pRect );

		pinUpdated( mPinOutputGeometry );
	}
}

void ScreenNode::updateScreenList()
{
	QStringList		ScrLst;

	for( QScreen *Screen : qApp->screens() )
	{
		ScrLst << Screen->name();
	}

	mValInputScreen->setChoices( ScrLst );
}

QScreen *ScreenNode::fromName( QString pName ) const
{
	for( QScreen *Screen : qApp->screens() )
	{
		if( Screen->name() == pName )
		{
			return( Screen );
		}
	}

	return( nullptr );
}
