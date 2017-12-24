#include <stdlib.h>

#if defined( Q_OS_UNIX )
#include <malloc/malloc.h>
#endif

#include "imagepin.h"
#include <QSettings>
#include <QImage>

ImagePin::ImagePin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin )
{
}

QString ImagePin::toString() const
{
	QStringList		ValLst;

//	if( mImage.isEmpty() )
//	{
//		ValLst << QString( "Empty Image" );
//	}
//	else
//	{
//		ValLst << QString( "%1x%2" ).arg( mImage.width() ).arg( mImage.height() );
//	}

//	QString		FmtStr = tr( "Format" );
//	QString		FmtVal = "Unknown";

//	switch( mImage.format() )
//	{
//		case FORMAT_UNKNOWN:		FmtVal = "FORMAT_UNKNOWN";		break;
//		case FORMAT_INTERNAL:		FmtVal = "FORMAT_INTERNAL";		break;
//		case FORMAT_RGB8:			FmtVal = "FORMAT_RGB8";			break;
//		case FORMAT_RGBA8:			FmtVal = "FORMAT_RGBA8";		break;
//		case FORMAT_BGR8:			FmtVal = "FORMAT_BGR8";			break;
//		case FORMAT_BGRA8:			FmtVal = "FORMAT_BGRA8";		break;
//		case FORMAT_YUYV422:		FmtVal = "FORMAT_YUYV422";		break;
//		case FORMAT_UYVY422:		FmtVal = "FORMAT_UYVY422";		break;
//		case FORMAT_YUV420P:		FmtVal = "FORMAT_YUV420P";		break;
//		case FORMAT_GRAY16:			FmtVal = "FORMAT_GRAY16";		break;
//		case FORMAT_GRAY8:			FmtVal = "FORMAT_GRAY8";		break;
//		case FORMAT_RG32:			FmtVal = "FORMAT_RG32";			break;
//		case FORMAT_DXT1:			FmtVal = "FORMAT_DXT1";			break;
//		case FORMAT_DXT5:			FmtVal = "FORMAT_DXT5";			break;
//		case FORMAT_YCoCg_DXT5:		FmtVal = "FORMAT_YCoCg_DXT5";	break;
//		case FORMAT_HSV8:			FmtVal = "FORMAT_HSV8";			break;
//		case FORMAT_R32S:			FmtVal = "FORMAT_R32S";			break;
//		case FORMAT_R32F:			FmtVal = "FORMAT_R32F";			break;
//		case FORMAT_NV12:			FmtVal = "FORMAT_NV12";			break;

//		default:
//			FmtStr = "Unknown";
//			break;
//	}

//	ValLst << QString( "%1: %2" ).arg( FmtStr ).arg( FmtVal );

	return( ValLst.join( '\n' ) );
}
