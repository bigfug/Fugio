#include "qrscannernode.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include <fugio/core/uuid.h>
#include <fugio/json/uuid.h>

#include <fugio/image/image.h>

QRScannerNode::QRScannerNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mQR( Q_NULLPTR ), mW( -1 ), mH( -1 )
{
	FUGID( PIN_INPUT_IMAGE, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_OUTPUT_CODES, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );

	mPinInputImage = pinInput( "Image", PIN_INPUT_IMAGE );

	mValOutputCodes = pinOutput<fugio::VariantInterface *>( "Codes", mPinOutputCodes, PID_JSON, PIN_OUTPUT_CODES );
}

bool QRScannerNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	if( ( mQR = quirc_new() ) == Q_NULLPTR )
	{
		return( false );
	}

	return( true );
}

bool QRScannerNode::deinitialise()
{
	if( mQR )
	{
		quirc_destroy( mQR );

		mQR = Q_NULLPTR;
	}

	return( NodeControlBase::deinitialise() );
}

void QRScannerNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	if( mPinInputImage->isUpdated( pTimeStamp ) )
	{
		fugio::Image	SrcImg = variant<fugio::Image>( mPinInputImage );

		if( !SrcImg.isValid() || SrcImg.isEmpty() )
		{
			mNode->setStatusMessage( tr( "Empty/invalid input image" ) );

			mNode->setStatus( fugio::NodeInterface::Error );

			return;
		}

		if( SrcImg.format() != fugio::ImageFormat::GRAY8 )
		{
			mNode->setStatusMessage( tr( "QR Scanner needs a grayscale image" ) );

			mNode->setStatus( fugio::NodeInterface::Error );
		}

		mNode->setStatusMessage( QString() );

		mNode->setStatus( fugio::NodeInterface::Initialised );

		// Resize the image if needed

		if( mW != SrcImg.width() || mH != SrcImg.height() )
		{
			if( quirc_resize( mQR, SrcImg.width(), SrcImg.height() ) != 0 )
			{
				mNode->setStatusMessage( tr( "Couldn't resize image to %1,%2" ).arg( SrcImg.width(), SrcImg.height() ) );

				mNode->setStatus( fugio::NodeInterface::Error );

				return;
			}

			mW = SrcImg.width();
			mH = SrcImg.height();
		}

		// Begin detection

		uint8_t	*DstPtr = quirc_begin( mQR, 0, 0 );

		if( !DstPtr )
		{
			return;
		}

		// Copy the source image into the processing buffer

		if( SrcImg.lineSize( 0 ) == SrcImg.width() )
		{
			memcpy( DstPtr, SrcImg.buffer( 0 ), SrcImg.bufferSize( 0 ) );
		}
		else
		{
			const uint8_t	*SrcPtr = SrcImg.buffer( 0 );

			for( int y = 0 ; y < SrcImg.height() ; y++, DstPtr += SrcImg.width(), SrcPtr += SrcImg.lineSize( 0 ) )
			{
				memcpy( DstPtr, SrcPtr, SrcImg.lineSize( 0 ) );
			}
		}

		// End detection

		quirc_end( mQR );

		QJsonArray		JsonArray;

		int		Count = quirc_count( mQR );

		for( int i = 0 ; i < Count ; i++ )
		{
			struct quirc_code	Code;
			struct quirc_data	Data;

			QJsonObject			JsonObject;

			quirc_extract( mQR, i, &Code );

			if( true )
			{
				QJsonArray			Corners;

				for( int i = 0 ; i < 4 ; i++ )
				{
					QJsonObject		Corner;

					Corner.insert( "x", Code.corners[ i ].x );
					Corner.insert( "y", Code.corners[ i ].y );

					Corners << Corner;
				}

				JsonObject.insert( "corners", Corners );
			}

			JsonObject.insert( "size", Code.size );

			if( !quirc_decode( &Code, &Data ) )
			{
				JsonObject.insert( "version", Data.version );

				switch( Data.ecc_level )
				{
					case QUIRC_ECC_LEVEL_M:
						JsonObject.insert( "ecc_level", "M" );
						break;

					case QUIRC_ECC_LEVEL_L:
						JsonObject.insert( "ecc_level", "L" );
						break;

					case QUIRC_ECC_LEVEL_H:
						JsonObject.insert( "ecc_level", "H" );
						break;

					case QUIRC_ECC_LEVEL_Q:
						JsonObject.insert( "ecc_level", "Q" );
						break;
				}

				switch( Data.data_type )
				{
					case QUIRC_DATA_TYPE_NUMERIC:
						JsonObject.insert( "data_type", "numeric" );
						JsonObject.insert( "payload", QString::fromLatin1( (const char *)&Data.payload, Data.payload_len ) );
						break;

					case QUIRC_DATA_TYPE_ALPHA:
						JsonObject.insert( "data_type", "alpha" );
						JsonObject.insert( "payload", QString::fromLatin1( (const char *)&Data.payload, Data.payload_len ) );
						break;

					case QUIRC_DATA_TYPE_BYTE:
						JsonObject.insert( "data_type", "byte" );
						JsonObject.insert( "payload", QString::fromLatin1( (const char *)&Data.payload, Data.payload_len ) );
						break;

					case QUIRC_DATA_TYPE_KANJI:
						JsonObject.insert( "data_type", "kanji" );
						JsonObject.insert( "payload", QString::fromLatin1( (const char *)&Data.payload, Data.payload_len ) );
						break;
				}

				JsonObject.insert( "mask", Data.mask );

				switch( Data.eci )
				{
					case QUIRC_ECI_ISO_8859_1:
						JsonObject.insert( "eci", "ISO_8859_1" );
						break;
				}
			}

			JsonArray << JsonObject;
		}

		mValOutputCodes->setVariant( QJsonDocument( JsonArray ) );

		pinUpdated( mPinOutputCodes );
	}
}

