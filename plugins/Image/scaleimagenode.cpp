#include "scaleimagenode.h"

#include <QScreen>
#include <QGuiApplication>
#include <QPixmap>

#include <fugio/core/uuid.h>
#include <fugio/image/uuid.h>

#include <fugio/image/image.h>

#include <fugio/performance.h>

ScaleImageNode::ScaleImageNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mPinInputImage = pinInput( "Image" );

	mPinWidth = pinInput( "Width" );

	mPinHeight = pinInput( "Height" );

	mValOutputImage = pinOutput<fugio::VariantInterface *>( "Image", mPinOutputImage, PID_IMAGE );

	mPinOutputImage->setDescription( tr( "The scaled image" ) );
}

void ScaleImageNode::inputsUpdated( qint64 pTimeStamp )
{
	fugio::Image			 SRC = variant<fugio::Image>( mPinInputImage );
	const QImage			 IMG = SRC.image();

	if( !SRC.isValid() || IMG.isNull() )
	{
		mNode->setStatus( fugio::NodeInterface::Warning );
		mNode->setStatusMessage( tr( "Input image is not valid" ) );

		return;
	}

	if( mNode->status() != fugio::NodeInterface::Initialised )
	{
		mNode->setStatus( fugio::NodeInterface::Initialised );
		mNode->setStatusMessage( QString() );
	}

	fugio::Performance		Perf( mNode, "inputsUpdated", pTimeStamp );

	qreal			 Xscl = variant( mPinWidth  ).toReal();
	qreal			 Yscl = variant( mPinHeight ).toReal();

	if( Xscl <= 1.0 )
	{
		Xscl *= qreal( SRC.size().width() );
	}

	if( Yscl <= 1.0 )
	{
		Yscl *= qreal( SRC.size().height() );
	}

	QSize			NewSze( Xscl, Yscl );

	if( !NewSze.width() && !NewSze.height() )
	{
		return;
	}

	QImage			NewImg;

	if( NewSze.width() > 0 && NewSze.height() == 0 )
	{
		NewImg = SRC.image().scaledToWidth( NewSze.width(), Qt::SmoothTransformation );
	}
	else if( NewSze.height() > 0 && NewSze.width() == 0 )
	{
		NewImg = SRC.image().scaledToHeight( NewSze.height(), Qt::SmoothTransformation );
	}
	else
	{
		NewImg = SRC.image().scaled( NewSze, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
	}

	if( NewImg.isNull() )
	{
		return;
	}

	NewSze = NewImg.size();

	fugio::Image	DST = mValOutputImage->variant().value<fugio::Image>();

	DST.setSize( NewImg.width(), NewImg.height() );

	DST.setLineSize( 0, NewImg.bytesPerLine() );

	if( NewImg.format() == QImage::Format_RGB32 )
	{
		DST.setFormat( fugio::ImageFormat::BGRA8 );
	}
	else if( NewImg.format() == QImage::Format_ARGB32 || NewImg.format() == QImage::Format_ARGB32_Premultiplied )
	{
		DST.setFormat( fugio::ImageFormat::BGRA8 );
	}
	else if( NewImg.format() == QImage::Format_RGB888 )
	{
		DST.setFormat( fugio::ImageFormat::RGB8 );
	}
	else
	{
		return;
	}

	memcpy( DST.internalBuffer( 0 ), NewImg.constBits(), NewImg.byteCount() );

	pinUpdated( mPinOutputImage );
}
