#include "imagepreviewnode.h"

//#include <fugio/core/uuid.h>

#include <QMainWindow>
#include <QPushButton>
#include <QSettings>

#include "fugio/global_interface.h"
#include "fugio/context_interface.h"
#include <fugio/core/uuid.h>
#include <fugio/context_signals.h>
#include <fugio/performance.h>
#include <fugio/editor_interface.h>

#include "imagepreview.h"

#define MAX_SIZE (480)

ImagePreviewNode::ImagePreviewNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mDockWidget( 0 ), mGUI( 0 ), mDockArea( Qt::BottomDockWidgetArea )
{
	FUGID( PIN_INPUT_IMAGE,				"9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_OUTPUT_EVENTS,			"1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_OUTPUT_IMAGE_SIZE,		"9C02B1DD-DAE9-4011-858C-C050FA3D9E7C" );

	mPinImage = pinInput( "Image", PIN_INPUT_IMAGE );

	mValOutputEvents = pinOutput<fugio::InputEventsInterface *>( "Events", mPinOutputEvents, PID_INPUT_EVENTS, PIN_OUTPUT_EVENTS );

	mValOutputSize = pinOutput<fugio::VariantInterface *>( "Image Size", mPinOutputSize, PID_SIZE, PIN_OUTPUT_IMAGE_SIZE );
}

QWidget *ImagePreviewNode::gui()
{
	QPushButton		*GUI = new QPushButton( "Show..." );

	connect( GUI, SIGNAL(clicked()), this, SLOT(onShowClicked()) );

	return( GUI );
}

void ImagePreviewNode::inputsUpdated( qint64 pTimeStamp )
{
	fugio::Performance	Perf( mNode, "inputsUpdated", pTimeStamp );

	NodeControlBase::inputsUpdated( pTimeStamp );

	if( mPinImage->isUpdated( pTimeStamp ) )
	{
		fugio::Image	SrcImg = variant<fugio::Image>( mPinImage );

		if( SrcImg.isValid() )
		{
			const QImage		Image = SrcImg.image();
			QPixmap				Pixmap;

			if( Image.width() > MAX_SIZE || Image.height() > MAX_SIZE )
			{
				Pixmap = QPixmap::fromImage( Image ).scaled( MAX_SIZE, MAX_SIZE, Qt::KeepAspectRatio );
			}
			else
			{
				Pixmap = QPixmap::fromImage( Image );
			}

			mGUI->setPixmap( Pixmap );

			if( Pixmap.size() != mValOutputSize->variant().toSize() )
			{
				mValOutputSize->setVariant( Pixmap.size() );

				pinUpdated( mPinOutputSize );
			}
		}
	}
}

bool ImagePreviewNode::initialise( void )
{
	if( !fugio::NodeControlBase::initialise() )
	{
		return( false );
	}

	fugio::EditorInterface	*EI = qobject_cast<fugio::EditorInterface *>( mNode->context()->global()->findInterface( IID_EDITOR ) );

	if( EI )
	{
		if( ( mDockWidget = new QDockWidget( QString( "Image Preview: %1" ).arg( mNode->name() ), EI->mainWindow() ) ) == 0 )
		{
			return( false );
		}

		mDockWidget->setObjectName( mNode->uuid().toString() );

		if( ( mGUI = new ImagePreview( mDockWidget ) ) == 0 )
		{
			return( false );
		}

		mGUI->setInputEventsInterface( mValOutputEvents );

		mDockWidget->setWidget( mGUI );

		EI->mainWindow()->addDockWidget( mDockArea, mDockWidget );

		connect( mNode->context()->qobject(), SIGNAL(frameInitialise()), this, SLOT(contextFrameInitialise()) );
	}

	return( true );
}

bool ImagePreviewNode::deinitialise( void )
{
	if( mNode->context() )
	{
		mNode->context()->qobject()->disconnect( this );
	}

	if( mDockWidget )
	{
		mDockWidget->deleteLater();

		mDockWidget = 0;

		mGUI = 0;
	}

	return( true );
}

void ImagePreviewNode::loadSettings( QSettings &pSettings )
{
	mDockArea = (Qt::DockWidgetArea)pSettings.value( "dockarea", int( mDockArea ) ).toInt();
}

void ImagePreviewNode::saveSettings( QSettings &pSettings ) const
{
	pSettings.setValue( "dockarea", int( mDockArea ) );
}

void ImagePreviewNode::onShowClicked( void )
{
	if( !mGUI )
	{
		return;
	}

	if( mDockWidget->isHidden() )
	{
		mDockWidget->show();
	}
}

void ImagePreviewNode::contextFrameInitialise()
{
	if( mValOutputEvents )
	{
		mValOutputEvents->inputFrameInitialise();
	}
}
