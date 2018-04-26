#include "imagesavenode.h"

#include <QDir>
#include <QImage>
#include <QStandardPaths>

#include <fugio/core/uuid.h>
#include <fugio/image/uuid.h>

#include <fugio/image/image.h>
#include <fugio/colour/colour_interface.h>

#include <fugio/performance.h>

ImageSaveNode::ImageSaveNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mFileNumber( 0 )
{
	FUGID( PIN_INPUT_IMAGE,		"9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_FORMAT,	"1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_INPUT_DIRECTORY, "249f2932-f483-422f-b811-ab679f006381" );
	FUGID( PIN_INPUT_OVERWRITE, "ce8d578e-c5a4-422f-b3c4-a1bdf40facdb" );
	FUGID( PIN_OUTPUT_FILENAME, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );
	FUGID( PIN_INPUT_RESET,		"51297977-7b4b-4e08-9dea-89a8add4abe0" );
	FUGID( PIN_INPUT_NUMBER,	"5bd10861-774e-4775-9269-ca1d0513408e" );

	mPinInputImage = pinInput( "Image", PIN_INPUT_IMAGE );

	mPinInputImage->registerPinInputType( PID_IMAGE );

	mPinInputDirectory = pinInput( "Directory", PIN_INPUT_DIRECTORY );

	QString		ImgDir = QStandardPaths::writableLocation( QStandardPaths::PicturesLocation );
	QString		OutNam = "Fugio Saved Images";

	mPinInputDirectory->setValue( QDir( ImgDir ).absoluteFilePath( OutNam ) );

	mPinInputOverwrite = pinInput( "Overwrite", PIN_INPUT_OVERWRITE );

	mPinInputOverwrite->setValue( false );

	mPinInputReset = pinInput( "Reset", PIN_INPUT_RESET );

	mPinInputNumber = pinInput( "Number", PIN_INPUT_NUMBER );

	mValInputFormat = pinInput<fugio::ChoiceInterface *>( "Format", mPinInputFormat, PID_CHOICE, PIN_INPUT_FORMAT );

	QStringList		FormatNames;

	FormatNames << "JPG";
	FormatNames << "TIF";
	FormatNames << "PNG";

	mValInputFormat->setChoices( FormatNames );

	mPinInputFormat->setValue( "PNG" );

	mValOutputFilename = pinOutput<fugio::VariantInterface *>( "Filename", mPinOutputFilename, PID_STRING, PIN_OUTPUT_FILENAME );
}

void ImageSaveNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	if( !pTimeStamp )
	{
		return;
	}

	if( mPinInputReset->isUpdated( pTimeStamp ) )
	{
		mFileNumber = 0;
	}

	if( mPinInputNumber->isConnected() )
	{
		mFileNumber = variant( mPinInputNumber ).toInt();
	}

	if( mPinInputImage->isUpdated( pTimeStamp ) )
	{
		fugio::Image	SrcImg = variant<fugio::Image>( mPinInputImage );

		if( !SrcImg.isValid() )
		{
			mNode->setStatus( fugio::NodeInterface::Warning );

			return;
		}

		fugio::Performance	Perf( mNode, "inputsUpdated", pTimeStamp );

		QDir		OutDir( variant( mPinInputDirectory ).toString() );

		if( !OutDir.exists() )
		{
			if( !OutDir.mkpath( "dummy" ) )
			{
				mNode->setStatus( fugio::NodeInterface::Error );
				mNode->setStatusMessage( tr( "Can't create path %1" ).arg( OutDir.absolutePath() ) );

				return;
			}

			OutDir.rmdir( "dummy" );
		}

		const bool	Overwrite = variant( mPinInputOverwrite ).toBool();
		QString		OutExt = variant( mPinInputFormat ).toString().toLower();
		QString		OutNam;

		while( true )
		{
			OutNam = QString( "%1.%2" ).arg( mFileNumber, int( 5 ), int( 10 ), QChar( '0' ) ).arg( OutExt );

			OutNam = OutDir.absoluteFilePath( OutNam );

			if( !Overwrite && QFile::exists( OutNam ) )
			{
				mFileNumber++;
			}
			else
			{
				break;
			}
		}

		if( SrcImg.image().save( OutNam ) )
		{
			mFileNumber++;

			mNode->setStatus( fugio::NodeInterface::Initialised );
		}
		else
		{
			mNode->setStatus( fugio::NodeInterface::Error );
		}

		mValOutputFilename->setVariant( OutNam );

		pinUpdated( mPinOutputFilename );
	}
}
