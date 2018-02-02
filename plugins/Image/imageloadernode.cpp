
#include "imageloadernode.h"

#include <QUrl>
#include <QImageReader>

#include "fugio/global_interface.h"
#include "fugio/context_interface.h"
#include <fugio/node_interface.h>
#include <fugio/core/variant_interface.h>
#include <fugio/file/filename_interface.h>

#include <fugio/image/uuid.h>
#include <fugio/image/image.h>

ImageLoaderNode::ImageLoaderNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_FILENAME, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_OUTPUT_IMAGE, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );

	mValInputFilename = pinInput<fugio::FilenameInterface *>( "Filename", mPinFileName, PID_FILENAME, PIN_INPUT_FILENAME );

	mImage = pinOutput<fugio::VariantInterface *>( "Image", mPinImage, PID_IMAGE, PIN_OUTPUT_IMAGE );
}

QWidget *ImageLoaderNode::gui( void )
{
	QLabel		*GUI = new QLabel();

	GUI->setFixedSize( 80, 80 );

	connect( this, SIGNAL(pixmapUpdated(QPixmap)), GUI, SLOT(setPixmap(QPixmap)) );

	if( !mImageData.isNull() )
	{
		GUI->setPixmap( QPixmap::fromImage( mImageData.scaledToWidth( 80 ) ) );
	}

	return( GUI );
}

void ImageLoaderNode::inputsUpdated( qint64 pTimeStamp )
{
	QString		ImageFileName;

	NodeControlBase::inputsUpdated( pTimeStamp );

	QUrl		ImageUrl;

	if( mPinFileName->isUpdated( pTimeStamp ) )
	{
		fugio::FilenameInterface		*F = input<fugio::FilenameInterface *>( mPinFileName );

		if( F )
		{
			ImageFileName = F->filename();

			ImageUrl = QUrl::fromLocalFile( ImageFileName );
		}
		else
		{
			ImageFileName = variant( mPinFileName ).toString();
		}
	}

	if( ImageFileName.isEmpty() )
	{
		return;
	}

	if( ImageUrl.isEmpty() )
	{
		ImageUrl = QUrl::fromLocalFile( ImageFileName );
	}

	if( !ImageUrl.isLocalFile() )
	{
		return;
	}

	QImageReader	ImageReader( ImageUrl.toLocalFile() );

	QImage			IMG = ImageReader.read();

	if( IMG.isNull() )
	{
		qWarning() << ImageUrl.toLocalFile() << ImageReader.errorString();

		//qDebug() << QImageReader::supportedImageFormats();

		return;
	}

	mImageData = IMG;

	fugio::Image		NewImg = mImage->variant().value<fugio::Image>();

	NewImg.setSize( mImageData.width(), mImageData.height() );

	NewImg.setLineSize( 0, mImageData.bytesPerLine() );

	if( mImageData.format() == QImage::Format_RGB32 )
	{
		NewImg.setFormat( fugio::ImageFormat::BGRA8 );
	}
	else if( mImageData.format() == QImage::Format_ARGB32 )
	{
		NewImg.setFormat( fugio::ImageFormat::BGRA8 );
	}

	memcpy( NewImg.internalBuffer( 0 ), mImageData.constBits(), mImageData.byteCount() );

	pinUpdated( mPinImage );

	emit pixmapUpdated( QPixmap::fromImage( mImageData.scaledToWidth( 80 ) ) );
}
