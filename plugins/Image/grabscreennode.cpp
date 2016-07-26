#include "grabscreennode.h"

#include <QScreen>
#include <QGuiApplication>
#include <QPixmap>

#include <fugio/core/uuid.h>
#include <fugio/image/uuid.h>

#include <fugio/image/image_interface.h>

GrabScreenNode::GrabScreenNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mPinTrigger = pinInput( "Trigger" );

	mValOutput = pinOutput<fugio::ImageInterface *>( "Image", mPinOutput, PID_IMAGE );

	mPinOutput->setDescription( tr( "The screenshot image" ) );
}

void GrabScreenNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	QScreen		*Screen = QGuiApplication::primaryScreen();

	if( !Screen )
	{
		return;
	}

	QPixmap PM = Screen->grabWindow(0);

	if( PM.isNull() )
	{
		return;
	}

	mImage = PM.toImage();

	if( mImage.isNull() )
	{
		return;
	}

	mValOutput->setSize( mImage.width(), mImage.height() );

	mValOutput->setLineSize( 0, mImage.bytesPerLine() );

	if( mImage.format() == QImage::Format_RGB32 )
	{
		mValOutput->setFormat( fugio::ImageInterface::FORMAT_BGRA8 );
	}
	else if( mImage.format() == QImage::Format_ARGB32 )
	{
		mValOutput->setFormat( fugio::ImageInterface::FORMAT_BGRA8 );
	}
	else
	{
		return;
	}

	memcpy( mValOutput->internalBuffer( 0 ), mImage.constBits(), mImage.byteCount() );

	pinUpdated( mPinOutput );
}
