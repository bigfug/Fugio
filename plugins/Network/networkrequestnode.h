#ifndef NETWORKREQUESTNODE_H
#define NETWORKREQUESTNODE_H

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>

#include <fugio/nodecontrolbase.h>

#include <fugio/file/filename_interface.h>

class NetworkRequestNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Performs a network request" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Network_Request" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE NetworkRequestNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~NetworkRequestNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

protected slots:
	void replyReadReady( void );
	void replyFinished( void );
	void replyError( QNetworkReply::NetworkError pError );

	void networkSslErrors( QNetworkReply *pNetRep, QList<QSslError> pSslErr );

	void contextFrameStart( void );

	void request( const QUrl &pUrl );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputTrigger;
	QSharedPointer<fugio::PinInterface>			 mPinInputUrl;

	QSharedPointer<fugio::PinInterface>			 mPinOutput;
	fugio::FilenameInterface					*mValOutput;

	QUrl										 mUrl;
	QNetworkReply								*mNetRep;
	QFile										*mTempFile;
	QFile										 mTempFile1;
	QFile										 mTempFile2;

	QString										 mFilename;

	// NodeControlInterface interface
public:
	virtual bool initialise() Q_DECL_OVERRIDE;
	virtual bool deinitialise() Q_DECL_OVERRIDE;
};

#endif // NETWORKREQUESTNODE_H
