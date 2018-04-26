#include "replacealphanode.h"

#include <QScreen>
#include <QGuiApplication>
#include <QPixmap>

#include <fugio/core/uuid.h>
#include <fugio/image/uuid.h>

#include <fugio/image/image.h>

#include <fugio/performance.h>

ReplaceAlphaNode::ReplaceAlphaNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_IMAGE,		"3E9A5B5C-3034-4D52-9F05-5B8E93FAE148" );
	FUGID( PIN_INPUT_ALPHA,		"96C3FCC5-3C70-4B2C-A91E-FA77E3D1D387" );
	FUGID( PIN_OUTPUT_IMAGE,	"26AE3C1B-5D5C-4DCD-9638-2971CBDF1AC5" );

	mPinInputImage = pinInput( "Image", PIN_INPUT_IMAGE );

	mPinInputAlpha = pinInput( "Alpha", PIN_INPUT_ALPHA );

	mValOutputImage = pinOutput<fugio::VariantInterface *>( "Image", mPinOutputImage, PID_IMAGE, PIN_OUTPUT_IMAGE );
}

void ReplaceAlphaNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	fugio::Image	SrcImg = variant<fugio::Image>( mPinInputImage );

	if( !SrcImg.isValid() )
	{
		return;
	}

	fugio::Image	AlpImg = variant<fugio::Image>( mPinInputAlpha );

	if( !AlpImg.isValid() )
	{
		return;
	}

	if( AlpImg.format() != fugio::ImageFormat::GRAY8 )
	{
		return;
	}

	if( SrcImg.size() != AlpImg.size() )
	{
		return;
	}

	fugio::Image	DstImg = mValOutputImage->variant().value<fugio::Image>();

	DstImg.setSize( SrcImg.size() );

	int		PixSze;

	switch( SrcImg.format() )
	{
		case fugio::ImageFormat::RGB8:
			PixSze = 3;
			DstImg.setFormat( fugio::ImageFormat::RGBA8 );
			break;

		case fugio::ImageFormat::RGBA8:
			PixSze = 4;
			DstImg.setFormat( fugio::ImageFormat::RGBA8 );
			break;

		case fugio::ImageFormat::BGR8:
			PixSze = 3;
			DstImg.setFormat( fugio::ImageFormat::BGRA8 );
			break;

		case fugio::ImageFormat::BGRA8:
			PixSze = 4;
			DstImg.setFormat( fugio::ImageFormat::BGRA8 );
			break;

		default:
			return;
	}

	DstImg.setLineSize( 0, 4 * SrcImg.width() );

	for( int y = 0 ; y < SrcImg.height() ; y++ )
	{
		const quint8	*SrcPtr = SrcImg.buffer( 0 ) + ( SrcImg.lineSize( 0 ) * y );
		const quint8	*AlpPtr = AlpImg.buffer( 0 ) + ( AlpImg.lineSize( 0 ) * y );
		quint8			*DstPtr = DstImg.internalBuffer( 0 ) + ( DstImg.lineSize( 0 ) * y );

		for( int x = 0 ; x < SrcImg.width() ; x++, SrcPtr += PixSze )
		{
			for( int c = 0 ; c < 3 ; c++ )
			{
				*DstPtr++ = SrcPtr[ c ];
			}

			*DstPtr++ = *AlpPtr++;
		}
	}

	pinUpdated( mPinOutputImage );
}
