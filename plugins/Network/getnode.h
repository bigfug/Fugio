#ifndef GETNODE_H
#define GETNODE_H

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>

#include <fugio/nodecontrolbase.h>

#include <fugio/file/filename_interface.h>

class GetNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Performs a network get" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Get_(HTTP)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE GetNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~GetNode( void ) {}

	// NodeControlInterface interface

	virtual bool initialise() Q_DECL_OVERRIDE;
	virtual bool deinitialise() Q_DECL_OVERRIDE;

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
};

#endif // GETNODE_H
