#include "imagenode.h"

#include <QUrl>
#include <QImageReader>

#include <fugio/image/uuid.h>
#include <fugio/core/uuid.h>

ImageNode::ImageNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mPinSize = pinInput( "Size" );

	mPinSize->registerPinInputType( PID_SIZE );

	mImage = pinOutput<fugio::ImageInterface *>( "Image", mPinImage, PID_IMAGE );
}

QWidget *ImageNode::gui( void )
{
	return( 0 );
}

void ImageNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	QSize		ImgSze = variant( mPinSize ).toSize();

	if( ImgSze != mImage->size() )
	{
		mImage->setFormat( fugio::ImageInterface::FORMAT_BGRA8 );
		mImage->setLineSize( 0, 4 * ImgSze.width() );
		mImage->setSize( ImgSze.width(), ImgSze.height() );

		pinUpdated( mPinImage );
	}
}
