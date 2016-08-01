#include "networkrequestnode.h"

#include "networkplugin.h"

#include <QCoreApplication>
#include <QDir>

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

	QDir		TempPath( QDir::tempPath() );

	mTempFile1.setFileName( TempPath.absoluteFilePath( QString( "%1.1" ).arg( mNode->uuid().toString() ) ) );
	mTempFile2.setFileName( TempPath.absoluteFilePath( QString( "%1.2" ).arg( mNode->uuid().toString() ) ) );
}

bool NetworkRequestNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	QNetworkAccessManager	*NAM = NetworkPlugin::nam();

	connect( NAM, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), this, SLOT(networkSslErrors(QNetworkReply*,QList<QSslError>)) );

	return( true );
}

bool NetworkRequestNode::deinitialise()
{
	QNetworkAccessManager	*NAM = NetworkPlugin::nam();

	NAM->disconnect( this );

	mTempFile1.remove();
	mTempFile2.remove();

	return( NodeControlBase::deinitialise() );
}

void NetworkRequestNode::inputsUpdated( qint64 pTimeStamp )
{
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
			mTempFile2.remove();

			mTempFile = &mTempFile2;
		}
		else
		{
			mTempFile1.remove();

			mTempFile = &mTempFile1;
		}

		if( mTempFile->open( QFile::WriteOnly ) )
		{
			request( mUrl );
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
	QUrl								 NetUrl = mNetRep->url();
	QNetworkReply::NetworkError			 NetErr = mNetRep->error();

	if( NetErr != QNetworkReply::NoError )
	{
		const QVariant RedirectionTarget = mNetRep->attribute( QNetworkRequest::RedirectionTargetAttribute );

		mNetRep->deleteLater();

		mNetRep = nullptr;

		if( !RedirectionTarget.isNull() )
		{
			const QUrl RedirectedUrl = NetUrl.resolved( RedirectionTarget.toUrl() );

			request( RedirectedUrl );

			return;
		}
	}

	mNetRep->deleteLater();

	mNetRep = nullptr;

	mTempFile->close();

	mFilename = mTempFile->fileName();

	qDebug() << mFilename;

	mNode->setStatus( fugio::NodeInterface::Initialised );
	mNode->setStatusMessage( tr( "Received %1 bytes" ).arg( mTempFile->size() ) );

	if( NetErr == QNetworkReply::NoError )
	{
		connect( mNode->context()->qobject(), SIGNAL(frameStart()), this, SLOT(contextFrameStart()) );
	}
}

void NetworkRequestNode::replyError( QNetworkReply::NetworkError )
{
	mNode->setStatus( fugio::NodeInterface::Error );
	mNode->setStatusMessage( mNetRep->errorString() );
}

void NetworkRequestNode::networkSslErrors( QNetworkReply *pNetRep, QList<QSslError> pSslErr )
{
	if( pNetRep == mNetRep )
	{
		QStringList		ErrLst;

		for( QSslError E : pSslErr )
		{
			ErrLst << E.errorString();
		}

		mNode->setStatus( fugio::NodeInterface::Error );
		mNode->setStatusMessage( ErrLst.join( '\n' ) );
	}
}

void NetworkRequestNode::contextFrameStart()
{
	mValOutput->setFilename( mFilename );

	pinUpdated( mPinOutput );

	disconnect( mNode->context()->qobject(), SIGNAL(frameStart()), this, SLOT(contextFrameStart()) );
}

void NetworkRequestNode::request( const QUrl &pUrl )
{
	QNetworkAccessManager	*NAM = NetworkPlugin::nam();

	QNetworkRequest			 NetReq( pUrl );

	NetReq.setRawHeader( "User-Agent", ( QString( "Fugio/%1 (bigfug.com)" ).arg( qApp->applicationVersion() ) ).toLatin1() );

	if( ( mNetRep = NAM->get( NetReq ) ) != nullptr )
	{
		connect( mNetRep, SIGNAL(readyRead()), this, SLOT(replyReadReady()) );
		connect( mNetRep, SIGNAL(finished()), this, SLOT(replyFinished()) );
		connect( mNetRep, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(replyError(QNetworkReply::NetworkError)) );
	}
}
