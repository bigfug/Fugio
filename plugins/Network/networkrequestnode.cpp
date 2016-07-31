#include "networkrequestnode.h"

#include "networkplugin.h"

#include <fugio/core/uuid.h>

NetworkRequestNode::NetworkRequestNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mTempFile( 0 )
{
	FUGID( PIN_INPUT_URL, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );

	//FUGID( PIN_XXX_XXX, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	//FUGID( PIN_XXX_XXX, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );

	FUGID( PIN_OUTPUT_OUTPUT, "249f2932-f483-422f-b811-ab679f006381" );

	mPinInputTrigger = pinInput( "Trigger", PID_FUGIO_NODE_TRIGGER );

	mPinInputUrl = pinInput( "URL", PIN_INPUT_URL );

	mValOutput = pinOutput<fugio::VariantInterface *>( "Output", mPinOutput, PID_BYTEARRAY, PIN_OUTPUT_OUTPUT );
}

void NetworkRequestNode::inputsUpdated( qint64 pTimeStamp )
{
	QNetworkAccessManager	*NAM = NetworkPlugin::nam();

	QUrl					 URL( variant( mPinInputUrl ).toString() );

	if( !URL.isValid() )
	{
		return;
	}

	if( mPinInputTrigger->isUpdated( pTimeStamp ) )
	{
		if( !mTempFile )
		{
			mTempFile = new QTemporaryFile( this );

			if( mTempFile )
			{
				if( mTempFile->open() )
				{
					QNetworkRequest		 NetReq( URL );
					QNetworkReply		*NetRep;

					if( ( NetRep = NAM->get( NetReq ) ) != nullptr )
					{
						connect( NetRep, SIGNAL(readyRead()), this, SLOT(replyReadReady()) );
						connect( NetRep, SIGNAL(finished()), this, SLOT(replyFinished()) );
					}
				}
				else
				{
					mTempFile->deleteLater();

					mTempFile = nullptr;
				}
			}
		}
	}
}

void NetworkRequestNode::replyReadReady()
{
	QNetworkReply		*NetRep = qobject_cast<QNetworkReply *>( sender() );

	if( mTempFile )
	{
		mTempFile->write( NetRep->readAll() );
	}
}

void NetworkRequestNode::replyFinished()
{
	QNetworkAccessManager	*NAM = NetworkPlugin::nam();
	QNetworkReply			*NetRep = qobject_cast<QNetworkReply *>( sender() );
	QUrl					 NetUrl = NetRep->url();

	if( NetRep->error() != QNetworkReply::NoError )
	{
		const QVariant RedirectionTarget = NetRep->attribute( QNetworkRequest::RedirectionTargetAttribute );

		NetRep->deleteLater();

		NetRep = nullptr;

		if( !RedirectionTarget.isNull() )
		{
			const QUrl RedirectedUrl = NetUrl.resolved( RedirectionTarget.toUrl() );

			QNetworkRequest		 NetReq( RedirectedUrl );

			if( ( NetRep = NAM->get( NetReq ) ) != nullptr )
			{
				connect( NetRep, SIGNAL(readyRead()), this, SLOT(replyReadReady()) );
				connect( NetRep, SIGNAL(finished()), this, SLOT(replyFinished()) );

				return;
			}
		}
	}

	NetRep->deleteLater();

	NetRep = nullptr;

	if( mTempFile )
	{
		if( mTempFile->reset() )
		{
			QByteArray	RepDat = mTempFile->readAll();

			mValOutput->setVariant( RepDat );

			pinUpdated( mPinOutput );
		}

		delete mTempFile;

		mTempFile = nullptr;
	}
}
