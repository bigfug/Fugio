#include "imagesizenode.h"

#include <fugio/core/uuid.h>
#include <fugio/image/uuid.h>

#include <fugio/image/image.h>
#include <fugio/colour/colour_interface.h>

#include <fugio/performance.h>

ImageSizeNode::ImageSizeNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	static const QUuid	ID_IMAGE = QUuid( "{34404B8E-B965-4C72-8E4D-8ED8ADAFF653}" );
	static const QUuid	ID_SIZE  = QUuid( "{19B78606-D8B3-4075-A9D5-B5C082CC3DBF}" );

	mPinInputImage = pinInput( "Image", ID_IMAGE );

	mPinInputImage->registerPinInputType( PID_IMAGE );

	mValOutputSize = pinOutput<fugio::VariantInterface *>( "Size", mPinOutputSize, PID_SIZE, ID_SIZE );
}

void ImageSizeNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	fugio::Image	SrcImg = variant<fugio::Image>( mPinInputImage );

	if( !SrcImg.isValid() )
	{
		mNode->setStatus( fugio::NodeInterface::Warning );

		return;
	}

	mNode->setStatus( fugio::NodeInterface::Initialised );

	mValOutputSize->setVariant( SrcImg.size() );

	pinUpdated( mPinOutputSize );
}
