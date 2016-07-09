#include "imagesavenode.h"

#include <QDir>
#include <QImage>

#include <fugio/core/uuid.h>
#include <fugio/image/uuid.h>

#include <fugio/image/image_interface.h>
#include <fugio/colour/colour_interface.h>

#include <fugio/performance.h>

ImageSaveNode::ImageSaveNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mFileNumber( 0 )
{
	mPinInputImage = pinInput( "Image" );

	mPinInputImage->registerPinInputType( PID_IMAGE );

	mValInputFormat = pinInput<fugio::ChoiceInterface *>( "Format", mPinInputFormat, PID_CHOICE );

	QStringList		FormatNames;

	FormatNames << "JPEG";
	FormatNames << "TIFF";
	FormatNames << "PNG";

	mValInputFormat->setChoices( FormatNames );

	mPinInputFormat->setValue( "PNG" );

	mValOutputFilename = pinOutput<fugio::VariantInterface *>( "Filename", mPinOutputFilename, PID_STRING );
}

void ImageSaveNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	fugio::ImageInterface	*SrcImg = input<fugio::ImageInterface *>( mPinInputImage );

	if( !SrcImg || SrcImg->size().isEmpty() )
	{
		mNode->setStatus( fugio::NodeInterface::Warning );

		return;
	}

	fugio::Performance	Perf( mNode, "inputsUpdated", pTimeStamp );

	QDir		OutDir( "/Users/bigfug/Desktop" );
	QString		OutExt = variant( mPinInputFormat ).toString().toLower();
	QString		OutNam = QString( "%1.%2" ).arg( mFileNumber, int( 5 ), int( 10 ), QChar( '0' ) ).arg( OutExt );

	OutNam = OutDir.absoluteFilePath( OutNam );

	if( SrcImg->image().save( OutNam ) )
	{
		mFileNumber++;
	}
	else
	{
		mNode->setStatus( fugio::NodeInterface::Error );
	}

	mNode->setStatus( fugio::NodeInterface::Initialised );

	mValOutputFilename->setVariant( OutNam );

	pinUpdated( mPinOutputFilename );
}
