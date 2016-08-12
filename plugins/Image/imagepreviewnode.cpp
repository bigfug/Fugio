#include "imagepreviewnode.h"

//#include <fugio/core/uuid.h>

#include <QMainWindow>
#include <QPushButton>
#include <QSettings>

#include "fugio/global_interface.h"
#include "fugio/context_interface.h"
#include <fugio/core/uuid.h>
#include <fugio/context_signals.h>
//#include <fugio/mouse_interface.h>
#include <fugio/performance.h>

#include "imagepreview.h"

ImagePreviewNode::ImagePreviewNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mDockWidget( 0 ), mGUI( 0 ), mDockArea( Qt::BottomDockWidgetArea )
{
	mPinImage = pinInput( "Image" );
}

ImagePreviewNode::~ImagePreviewNode( void )
{
	QMainWindow		*MainWindow = mNode->context()->global()->mainWindow();

	if( mDockWidget != 0 )
	{
		MainWindow->removeDockWidget( mDockWidget );

		delete mDockWidget;

		mDockWidget = 0;
	}
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

	if( mPinImage->isConnectedToActiveNode() && mPinImage->isUpdated( pTimeStamp ) )
	{
		fugio::ImageInterface		*SrcImg = input<fugio::ImageInterface *>( mPinImage );

		if( SrcImg && !SrcImg->size().isEmpty() )
		{
			mGUI->setPixmap( QPixmap::fromImage( SrcImg->image() ).scaled( 512, 512, Qt::KeepAspectRatio ) );
		}
	}
}

bool ImagePreviewNode::initialise( void )
{
	if( !fugio::NodeControlBase::initialise() )
	{
		return( false );
	}

	QMainWindow		*MainWindow = mNode->context()->global()->mainWindow();

	if( MainWindow != 0 )
	{
		if( ( mDockWidget = new QDockWidget( QString( "Image Preview: %1" ).arg( mNode->name() ), MainWindow ) ) == 0 )
		{
			return( false );
		}

		mDockWidget->setObjectName( mNode->uuid().toString() );

		if( ( mGUI = new ImagePreview( mDockWidget ) ) == 0 )
		{
			return( false );
		}

		mDockWidget->setWidget( mGUI );

		MainWindow->addDockWidget( mDockArea, mDockWidget );

		connect( mNode->context()->qobject(), SIGNAL(frameStart()), this, SLOT(onContextFrame()) );
	}

	return( true );
}

bool ImagePreviewNode::deinitialise( void )
{
	if( mDockWidget != 0 )
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
	if( mGUI == 0 )
	{
		return;
	}

	if( mDockWidget->isHidden() )
	{
		mDockWidget->show();
	}
}

void ImagePreviewNode::onContextFrame()
{
	if( !mGUI || !mGUI->isVisible() )
	{
		return;
	}

//	if( mGUI->mousePosition() != mValMousePosition->variant().toPointF() )
//	{
//		mValMousePosition->setVariant( mGUI->mousePosition() );

//		pinUpdated( mPinMousePosition );
//	}

//	if( mGUI->mouseLeft() != mValMouseLeft->variant().toBool() )
//	{
//		mValMouseLeft->setVariant( mGUI->mouseLeft() );

//		pinUpdated( mPinMouseLeft );
//	}
}
