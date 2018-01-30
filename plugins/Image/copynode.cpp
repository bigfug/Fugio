#include "copynode.h"

#include <QScreen>
#include <QGuiApplication>
#include <QPixmap>

#include <fugio/core/uuid.h>
#include <fugio/image/uuid.h>

#include <fugio/image/image.h>

#include <fugio/performance.h>

CopyNode::CopyNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_IMAGE,		"3E9A5B5C-3034-4D52-9F05-5B8E93FAE148" );
	FUGID( PIN_INPUT_RECT,		"96C3FCC5-3C70-4B2C-A91E-FA77E3D1D387" );
	FUGID( PIN_OUTPUT_IMAGE,	"26AE3C1B-5D5C-4DCD-9638-2971CBDF1AC5" );

	mPinInputImage = pinInput( "Image", PIN_INPUT_IMAGE );

	mPinInputRect = pinInput( "Area", PIN_INPUT_RECT );

	mValOutputImage = pinOutput<fugio::VariantInterface *>( "Image", mPinOutputImage, PID_IMAGE, PIN_OUTPUT_IMAGE );

	mPinOutputImage->setDescription( tr( "The copied image" ) );
}

void CopyNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	fugio::Image	SrcImg = variant<fugio::Image>( mPinInputImage );

	if( !SrcImg.isValid() )
	{
		return;
	}

	QRect		SrcRct = QRect( QPoint(), SrcImg.size() );
	QRect		DstRct = variant( mPinInputRect ).toRect();

	if( !DstRct.isValid() || DstRct.isEmpty() )
	{
		DstRct = SrcRct;
	}

	fugio::Image	DstImg = mValOutputImage->variant().value<fugio::Image>();

	DstImg.setSize( DstRct.width(), DstRct.height() );
	DstImg.setFormat( SrcImg.format() );
	DstImg.setInternalFormat( SrcImg.internalFormat() );

	if( DstRct == SrcRct )
	{
		for( int i = 0 ; i < 8 ; i++ )
		{
			DstImg.setLineSize( i, SrcImg.lineSize( i ) );
		}

		for( int i = 0 ; i < 8 ; i++ )
		{
			const int BufSiz = SrcImg.bufferSize( i );

			if( !BufSiz )
			{
				break;
			}

			memcpy( DstImg.internalBuffer( i ), SrcImg.buffer( i ), BufSiz );
		}
	}
	else
	{
		for( int i = 0 ; i < 8 ; i++ )
		{
			DstImg.setLineSize( i, DstRct.width() * fugio::Image::formatPixelByteCount( SrcImg.format(), i ) );
		}

		for( int i = 0 ; i < 8 && SrcImg.bufferSize( i ) ; i++ )
		{
			const int		 PixSze = fugio::Image::formatPixelByteCount( SrcImg.format(), i );
			const QRect		 SrcCpy( PixSze * DstRct.left(), SrcImg.lineSize( i ) * DstRct.top(), PixSze * DstRct.width(), 1 );
			const quint8	*SrcBuf = SrcImg.buffer( i );
			quint8			*DstBuf = DstImg.internalBuffer( i );
			const int		 BufSiz = SrcImg.bufferSize( i );

			for( int y = 0 ; y < DstRct.height() ; y++ )
			{
				int		SrcOff = SrcCpy.left() + SrcCpy.top() + SrcImg.lineSize( i ) * y;
				int		SrcCnt = std::min<int>( SrcCpy.width(), BufSiz - SrcOff );

				if( SrcCnt > 0 )
				{
					memcpy( DstBuf + y * SrcCpy.width(), SrcBuf + SrcOff, SrcCnt );
				}
			}
		}
	}

	pinUpdated( mPinOutputImage );
}
