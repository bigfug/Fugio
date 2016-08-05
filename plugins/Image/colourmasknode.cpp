#include "colourmasknode.h"

#include <fugio/core/uuid.h>
#include <fugio/image/uuid.h>

#include <fugio/image/image_interface.h>
#include <fugio/colour/colour_interface.h>

#include <fugio/performance.h>

ColourMaskNode::ColourMaskNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mPinInputImage = pinInput( "Image" );
	mPinColour     = pinInput( "Colour" );
	mPinHueMatch   = pinInput( "Hue Match" );

	mPinInputImage->registerPinInputType( PID_IMAGE );

	mPinColour->registerPinInputType( PID_COLOUR );

	mPinHueMatch->registerPinInputType( PID_FLOAT );

	mValOutputImage = pinOutput<fugio::ImageInterface *>( "Image", mPinOutputImage, PID_IMAGE );
}

void ColourMaskNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	fugio::ImageInterface	*SrcImg = input<fugio::ImageInterface *>( mPinInputImage );

	if( !SrcImg || !SrcImg->isValid() )
	{
		mNode->setStatus( fugio::NodeInterface::Warning );
		mNode->setStatusMessage( "Image is not valid" );

		return;
	}

	if( SrcImg->format() != fugio::ImageInterface::FORMAT_HSV8 )
	{
		mNode->setStatus( fugio::NodeInterface::Warning );
		mNode->setStatusMessage( "Image must be HSV8" );

		return;
	}

	if( mNode->status() != fugio::NodeInterface::Initialised )
	{
		mNode->setStatus( fugio::NodeInterface::Initialised );
		mNode->setStatusMessage( QString() );
	}

	fugio::Performance	Perf( mNode, "inputsUpdated", pTimeStamp );

	QColor		SrcCol = variant( mPinColour ).value<QColor>();

	qint16		HVal   = SrcCol.hsvHueF() * 180.0f;
	qint16		HVar   = variant( mPinHueMatch ).toFloat() * 90.0f;

	if( mValOutputImage->size() != SrcImg->size() )
	{
		mValOutputImage->setFormat( fugio::ImageInterface::FORMAT_GRAY8 );
		mValOutputImage->setSize( SrcImg->width(), SrcImg->height() );
		mValOutputImage->setLineSize( 0, SrcImg->width() );
	}

	for( int y = 0 ; y < SrcImg->height() ; y++ )
	{
		const quint8		*SrcPtr = SrcImg->buffer( 0 ) + SrcImg->lineSize( 0 ) * y;
		quint8				*DstPtr = mValOutputImage->internalBuffer( 0 ) + mValOutputImage->lineSize( 0 ) * y;

		for( int x = 0 ; x < SrcImg->width() ; x++ )
		{
			int		 Idx = x * 3;

			qint16	 HSrc = qint16( SrcPtr[ Idx + 0 ] );

			if( qAbs( HSrc - HVal ) <= HVar )
			{
				DstPtr[ x ] = 255;
			}
			else
			{
				DstPtr[ x ] = 0;
			}
		}
	}

	pinUpdated( mPinOutputImage );
}
