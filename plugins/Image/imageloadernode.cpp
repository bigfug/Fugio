
#include "imageloadernode.h"

#include <QUrl>
#include <QImageReader>

#include "fugio/global_interface.h"
#include "fugio/context_interface.h"
#include <fugio/node_interface.h>
#include <fugio/core/variant_interface.h>
#include <fugio/file/filename_interface.h>

#include <fugio/image/uuid.h>

ImageLoaderNode::ImageLoaderNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mPinFileName = pinInput( "FileName" );

	mImage = pinOutput<fugio::ImageInterface *>( "Image", mPinImage, PID_IMAGE );
}

ImageLoaderNode::~ImageLoaderNode( void )
{
}

QWidget *ImageLoaderNode::gui( void )
{
	QLabel		*GUI = new QLabel();

	GUI->setFixedSize( 80, 80 );

	connect( this, SIGNAL(pixmapUpdated(QPixmap)), GUI, SLOT(setPixmap(QPixmap)) );

	return( GUI );
}

void ImageLoaderNode::inputsUpdated( qint64 pTimeStamp )
{
	QString		ImageFileName;

	NodeControlBase::inputsUpdated( pTimeStamp );

	QUrl		ImageUrl;

	if( mPinFileName->isConnected() )
	{
		QSharedPointer<fugio::PinControlInterface>		 CTL = mPinFileName->connectedPin()->control();
		fugio::VariantInterface						*VAR = ( CTL == 0 ? 0 : qobject_cast<fugio::VariantInterface *>( CTL->qobject() ) );

		if( VAR != 0 )
		{
			ImageFileName = VAR->variant().toString();
		}

		fugio::FilenameInterface						*IFN = ( CTL == 0 ? 0 : qobject_cast<fugio::FilenameInterface *>( CTL->qobject() ) );

		if( IFN )
		{
			ImageFileName = IFN->filename();

			ImageUrl = QUrl::fromLocalFile( ImageFileName );
		}
	}
	else
	{
		ImageFileName = mPinFileName->value().toString();
	}

	if( ImageFileName.isEmpty() )
	{
		return;
	}

	if( ImageUrl.isEmpty() )
	{
		ImageUrl = QUrl( ImageFileName );
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

		return;
	}

	mImageData = IMG;

	mImage->setSize( mImageData.width(), mImageData.height() );

	mImage->setLineSize( 0, mImageData.bytesPerLine() );

	if( mImageData.format() == QImage::Format_RGB32 )
	{
		mImage->setFormat( fugio::ImageInterface::FORMAT_BGRA8 );
	}
	else if( mImageData.format() == QImage::Format_ARGB32 )
	{
		mImage->setFormat( fugio::ImageInterface::FORMAT_BGRA8 );
	}

	memcpy( mImage->internalBuffer( 0 ), mImageData.constBits(), mImageData.byteCount() );

	mNode->context()->pinUpdated( mPinImage );

	emit pixmapUpdated( QPixmap::fromImage( mImageData.scaledToWidth( 80 ) ) );
}
