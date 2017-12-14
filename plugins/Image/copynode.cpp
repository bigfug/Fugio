#include "copynode.h"

#include <QScreen>
#include <QGuiApplication>
#include <QPixmap>

#include <fugio/core/uuid.h>
#include <fugio/image/uuid.h>

#include <fugio/image/image_interface.h>

#include <fugio/performance.h>

CopyNode::CopyNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_IMAGE,		"3E9A5B5C-3034-4D52-9F05-5B8E93FAE148" )
	FUGID( PIN_OUTPUT_IMAGE,	"26AE3C1B-5D5C-4DCD-9638-2971CBDF1AC5" )

	mPinInputImage = pinInput( "Image", PIN_INPUT_IMAGE );

	mValOutputImage = pinOutput<fugio::ImageInterface *>( "Image", mPinOutputImage, PID_IMAGE, PIN_OUTPUT_IMAGE );

	mPinOutputImage->setDescription( tr( "The copied image" ) );
}

void CopyNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	fugio::ImageInterface	*SrcImg = input<fugio::ImageInterface *>( mPinInputImage );

	if( !SrcImg || !SrcImg->isValid() )
	{
		return;
	}

	mValOutputImage->setSize( SrcImg->size().width(), SrcImg->size().height() );
	mValOutputImage->setFormat( SrcImg->format() );
	mValOutputImage->setInternalFormat( SrcImg->internalFormat() );

	for( int i = 0 ; i < 8 ; i++ )
	{
		mValOutputImage->setLineSize( i, SrcImg->lineSize( i ) );
	}

	for( int i = 0 ; i < 8 ; i++ )
	{
		const int BufSiz = SrcImg->bufferSize( i );

		if( !BufSiz )
		{
			break;
		}

		memcpy( mValOutputImage->internalBuffer( i ), SrcImg->buffer( i ), BufSiz );
	}

	pinUpdated( mPinOutputImage );
}
