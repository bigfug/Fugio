#include "networkrequestnode.h"

#include "networkplugin.h"

#include <fugio/core/uuid.h>
#include <fugio/file/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/context_signals.h>

NetworkRequestNode::NetworkRequestNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mNetRep( 0 ), mTempFile( &mTempFile1 )
{
	FUGID( PIN_INPUT_URL, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );

	//FUGID( PIN_XXX_XXX, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );

	FUGID( PIN_OUTPUT_FILENAME, "249f2932-f483-422f-b811-ab679f006381" );
	//FUGID( PIN_OUTPUT_ERROR,	"B5F5E882-DC79-47E0-8129-FFC7D31B3C2B" );

	mPinInputTrigger = pinInput( "Trigger", PID_FUGIO_NODE_TRIGGER );

	mPinInputUrl = pinInput( "URL", PIN_INPUT_URL );

	mValOutput = pinOutput<fugio::FilenameInterface *>( "Filename", mPinOutput, PID_FILENAME, PIN_OUTPUT_FILENAME );
}

void NetworkRequestNode::inputsUpdated( qint64 pTimeStamp )
{
	QNetworkAccessManager	*NAM = NetworkPlugin::nam();
	bool					 Update = mPinInputTrigger->isUpdated( pTimeStamp );

	if( mPinInputUrl->isUpdated( pTimeStamp ) )
	{
		QUrl					 URL( variant( mPinInputUrl ).toString() );

		if( !URL.isValid() )
		{
			return;
		}

		if( URL != mUrl )
		{
			Update = true;

			mUrl = URL;
		}
	}

	if( Update )
	{
		if( mNetRep )
		{
			return;
		}

		if( mTempFile == &mTempFile1 )
		{
			if( !mTempFile2.fileName().isEmpty() )
			{
				mTempFile2.remove();
			}

			mTempFile = &mTempFile2;
		}
		else
		{
			if( !mTempFile1.fileName().isEmpty() )
			{
				mTempFile1.remove();
			}

			mTempFile = &mTempFile1;
		}

		if( mTempFile->open() )
		{
			QNetworkRequest		 NetReq( mUrl );

			if( ( mNetRep = NAM->get( NetReq ) ) != nullptr )
			{
				connect( mNetRep, SIGNAL(readyRead()), this, SLOT(replyReadReady()) );
				connect( mNetRep, SIGNAL(finished()), this, SLOT(replyFinished()) );
			}
		}
	}
}

void NetworkRequestNode::replyReadReady()
{
	if( mTempFile )
	{
		mTempFile->write( mNetRep->readAll() );
	}
}

void NetworkRequestNode::replyFinished()
{
	QNetworkAccessManager	*NAM = NetworkPlugin::nam();
	QUrl					 NetUrl = mNetRep->url();

	if( mNetRep->error() != QNetworkReply::NoError )
	{
		const QVariant RedirectionTarget = mNetRep->attribute( QNetworkRequest::RedirectionTargetAttribute );

		mNetRep->deleteLater();

		mNetRep = nullptr;

		if( !RedirectionTarget.isNull() )
		{
			const QUrl RedirectedUrl = NetUrl.resolved( RedirectionTarget.toUrl() );

			QNetworkRequest		 NetReq( RedirectedUrl );

			if( ( mNetRep = NAM->get( NetReq ) ) != nullptr )
			{
				connect( mNetRep, SIGNAL(readyRead()), this, SLOT(replyReadReady()) );
				connect( mNetRep, SIGNAL(finished()), this, SLOT(replyFinished()) );

				return;
			}
		}
	}

	mNetRep->deleteLater();

	mNetRep = nullptr;

	mTempFile->close();

	connect( mNode->context()->qobject(), SIGNAL(frameStart()), this, SLOT(contextFrameStart()) );
}

void NetworkRequestNode::contextFrameStart()
{
	mValOutput->setFilename( mTempFile->fileName() );

	pinUpdated( mPinOutput );

	disconnect( mNode->context()->qobject(), SIGNAL(frameStart()), this, SLOT(contextFrameStart()) );
}
