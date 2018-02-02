#include <stdlib.h>

#if defined( Q_OS_UNIX )
#include <malloc/malloc.h>
#endif

#include "imagepin.h"
#include <QSettings>
#include <QImage>

ImagePin::ImagePin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), VariantHelper( QMetaType::Type( qMetaTypeId<fugio::Image>() ), PID_IMAGE )
{
}

QString ImagePin::toString() const
{
	QStringList		ValLst;

	for( const fugio::Image &I : mValues )
	{
		if( I.isEmpty() )
		{
			ValLst << QString( "Empty Image" );
		}
		else
		{
			ValLst << QString( "%1x%2" ).arg( I.width() ).arg( I.height() );
		}

		QString		FmtStr = tr( "Format" );
		QString		FmtVal = "Unknown";

		switch( I.format() )
		{
			case fugio::ImageFormat::UNKNOWN:		FmtVal = "FORMAT_UNKNOWN";		break;
			case fugio::ImageFormat::INTERNAL:		FmtVal = "FORMAT_INTERNAL";		break;
			case fugio::ImageFormat::RGB8:			FmtVal = "FORMAT_RGB8";			break;
			case fugio::ImageFormat::RGBA8:			FmtVal = "FORMAT_RGBA8";		break;
			case fugio::ImageFormat::BGR8:			FmtVal = "FORMAT_BGR8";			break;
			case fugio::ImageFormat::BGRA8:			FmtVal = "FORMAT_BGRA8";		break;
			case fugio::ImageFormat::YUYV422:		FmtVal = "FORMAT_YUYV422";		break;
			case fugio::ImageFormat::UYVY422:		FmtVal = "FORMAT_UYVY422";		break;
			case fugio::ImageFormat::YUV420P:		FmtVal = "FORMAT_YUV420P";		break;
			case fugio::ImageFormat::GRAY16:			FmtVal = "FORMAT_GRAY16";		break;
			case fugio::ImageFormat::GRAY8:			FmtVal = "FORMAT_GRAY8";		break;
			case fugio::ImageFormat::RG32:			FmtVal = "FORMAT_RG32";			break;
			case fugio::ImageFormat::DXT1:			FmtVal = "FORMAT_DXT1";			break;
			case fugio::ImageFormat::DXT5:			FmtVal = "FORMAT_DXT5";			break;
			case fugio::ImageFormat::YCoCg_DXT5:		FmtVal = "FORMAT_YCoCg_DXT5";	break;
			case fugio::ImageFormat::HSV8:			FmtVal = "FORMAT_HSV8";			break;
			case fugio::ImageFormat::R32S:			FmtVal = "FORMAT_R32S";			break;
			case fugio::ImageFormat::R32F:			FmtVal = "FORMAT_R32F";			break;
			case fugio::ImageFormat::NV12:			FmtVal = "FORMAT_NV12";			break;

			default:
				FmtStr = "Unknown";
				break;
		}

		ValLst << QString( "%1: %2" ).arg( FmtStr ).arg( FmtVal );
	}

	return( ValLst.join( '\n' ) );
}
