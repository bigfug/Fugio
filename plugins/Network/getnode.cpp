#include "getnode.h"

#include "networkplugin.h"

#include <QCoreApplication>
#include <QDir>

#include <fugio/core/uuid.h>
#include <fugio/file/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/context_signals.h>

GetNode::GetNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mNetRep( 0 ), mTempFile( &mTempFile1 )
{
	FUGID( PIN_INPUT_URL, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_OUTPUT_FILENAME, "249f2932-f483-422f-b811-ab679f006381" );

	mPinInputTrigger = pinInput( "Trigger", PID_FUGIO_NODE_TRIGGER );

	mPinInputUrl = pinInput( "URL", PIN_INPUT_URL );

	mValOutput = pinOutput<fugio::FilenameInterface *>( "Filename", mPinOutput, PID_FILENAME, PIN_OUTPUT_FILENAME );

	QDir		TempPath( QDir::tempPath() );
	QUuid		TempUuid = QUuid::createUuid();

	mTempFile1.setFileName( TempPath.absoluteFilePath( QString( "%1.1" ).arg( TempUuid.toString() ) ) );
	mTempFile2.setFileName( TempPath.absoluteFilePath( QString( "%1.2" ).arg( TempUuid.toString() ) ) );
}

bool GetNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	QNetworkAccessManager	*NAM = NetworkPlugin::nam();

	connect( NAM, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), this, SLOT(networkSslErrors(QNetworkReply*,QList<QSslError>)) );

	return( true );
}

bool GetNode::deinitialise()
{
	QNetworkAccessManager	*NAM = NetworkPlugin::nam();

	NAM->disconnect( this );

	mTempFile1.remove();
	mTempFile2.remove();

	return( NodeControlBase::deinitialise() );
}

void GetNode::inputsUpdated( qint64 pTimeStamp )
{
	bool					 Update = mPinInputTrigger->isUpdated( pTimeStamp );

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

	if( !mUrl.isValid() )
	{
		return;
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

void GetNode::replyReadReady()
{
	if( mTempFile )
	{
		mTempFile->write( mNetRep->readAll() );
	}
}

void GetNode::replyFinished()
{
	QUrl								 NetUrl = mNetRep->url();
	QNetworkReply::NetworkError			 NetErr = mNetRep->error();

	if( NetErr != QNetworkReply::NoError )
	{
		const QVariant RedirectionTarget = mNetRep->attribute( QNetworkRequest::RedirectionTargetAttribute );

		mNetRep->disconnect( this );

		mNetRep->deleteLater();

		mNetRep = nullptr;

		if( !RedirectionTarget.isNull() )
		{
			const QUrl RedirectedUrl = NetUrl.resolved( RedirectionTarget.toUrl() );

			request( RedirectedUrl );
		}

		return;
	}

	mNetRep->disconnect( this );

	mNetRep->deleteLater();

	mNetRep = nullptr;

	mTempFile->close();

	mFilename = mTempFile->fileName();

	mNode->setStatus( fugio::NodeInterface::Initialised );
	mNode->setStatusMessage( tr( "Received %1 bytes" ).arg( mTempFile->size() ) );

	if( NetErr == QNetworkReply::NoError )
	{
		connect( mNode->context()->qobject(), SIGNAL(frameStart()), this, SLOT(contextFrameStart()) );
	}
}

void GetNode::replyError( QNetworkReply::NetworkError )
{
	mNode->setStatus( fugio::NodeInterface::Error );
	mNode->setStatusMessage( QString( "%1\n%2" ).arg( mNetRep->url().toDisplayString() ).arg( mNetRep->errorString() ) );
}

void GetNode::networkSslErrors( QNetworkReply *pNetRep, QList<QSslError> pSslErr )
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

void GetNode::contextFrameStart()
{
	mValOutput->setFilename( mFilename );

	pinUpdated( mPinOutput );

	disconnect( mNode->context()->qobject(), SIGNAL(frameStart()), this, SLOT(contextFrameStart()) );
}

void GetNode::request( const QUrl &pUrl )
{
	QNetworkAccessManager	*NAM = NetworkPlugin::nam();

	QNetworkRequest			 NetReq( pUrl );

	if( ( mNetRep = NAM->get( NetReq ) ) != nullptr )
	{
		//qDebug() << pUrl;

		connect( mNetRep, SIGNAL(readyRead()), this, SLOT(replyReadReady()) );
		connect( mNetRep, SIGNAL(finished()), this, SLOT(replyFinished()) );
		connect( mNetRep, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(replyError(QNetworkReply::NetworkError)) );
	}
}
