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

	mImage = pinOutput<fugio::VariantInterface *>( "Image", mPinImage, PID_IMAGE );
}

QWidget *ImageNode::gui( void )
{
	return( 0 );
}

void ImageNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	fugio::Image	DstImg = mImage->variant().value<fugio::Image>();

	QSize			ImgSze = variant( mPinSize ).toSize();

	if( ImgSze != DstImg.size() )
	{
		DstImg.setFormat( fugio::ImageFormat::BGRA8 );
		DstImg.setLineSize( 0, 4 * ImgSze.width() );
		DstImg.setSize( ImgSze.width(), ImgSze.height() );

		pinUpdated( mPinImage );
	}
}
