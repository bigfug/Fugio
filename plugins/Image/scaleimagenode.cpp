#include "scaleimagenode.h"

#include <QScreen>
#include <QGuiApplication>
#include <QPixmap>

#include <fugio/core/uuid.h>
#include <fugio/image/uuid.h>

#include <fugio/image/image_interface.h>

#include <fugio/performance.h>

ScaleImageNode::ScaleImageNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mPinInputImage = pinInput( "Image" );

	mPinWidth = pinInput( "Width" );

	mPinHeight = pinInput( "Height" );

	mValOutputImage = pinOutput<fugio::ImageInterface *>( "Image", mPinOutputImage, PID_IMAGE );

	mPinOutputImage->setDescription( tr( "The scaled image" ) );
}

void ScaleImageNode::inputsUpdated( qint64 pTimeStamp )
{
	if( !pTimeStamp )
	{
		return;
	}

	fugio::ImageInterface	*SRC = input<fugio::ImageInterface *>( mPinInputImage );

	if( !SRC || SRC->size().isEmpty() )
	{
		return;
	}

	fugio::Performance		Perf( mNode, "inputsUpdated", pTimeStamp );

	qreal			 Xscl = variant( mPinWidth  ).toReal();
	qreal			 Yscl = variant( mPinHeight ).toReal();

	if( Xscl <= 1.0 )
	{
		Xscl *= qreal( SRC->size().width() );
	}

	if( Yscl <= 1.0 )
	{
		Yscl *= qreal( SRC->size().height() );
	}

	QSize			NewSze( Xscl, Yscl );

	QImage			NewImg = SRC->image().scaled( NewSze, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );

	if( NewImg.isNull() )
	{
		return;
	}

	mValOutputImage->setSize( NewImg.width(), NewImg.height() );

	mValOutputImage->setLineSize( 0, NewImg.bytesPerLine() );

	if( NewImg.format() == QImage::Format_RGB32 )
	{
		mValOutputImage->setFormat( fugio::ImageInterface::FORMAT_BGRA8 );
	}
	else if( NewImg.format() == QImage::Format_ARGB32 || NewImg.format() == QImage::Format_ARGB32_Premultiplied )
	{
		mValOutputImage->setFormat( fugio::ImageInterface::FORMAT_BGRA8 );
	}
	else if( NewImg.format() == QImage::Format_RGB888 )
	{
		mValOutputImage->setFormat( fugio::ImageInterface::FORMAT_RGB8 );
	}
	else
	{
		return;
	}

	memcpy( mValOutputImage->internalBuffer( 0 ), NewImg.constBits(), NewImg.byteCount() );

	pinUpdated( mPinOutputImage );
}