#ifndef NETWORKREQUESTNODE_H
#define NETWORKREQUESTNODE_H

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTemporaryFile>

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

	void contextFrameStart( void );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputTrigger;
	QSharedPointer<fugio::PinInterface>			 mPinInputUrl;

	QSharedPointer<fugio::PinInterface>			 mPinOutput;
	fugio::FilenameInterface					*mValOutput;

	QUrl										 mUrl;
	QNetworkReply								*mNetRep;
	QTemporaryFile								*mTempFile;
	QTemporaryFile								 mTempFile1;
	QTemporaryFile								 mTempFile2;
};

#endif // NETWORKREQUESTNODE_H
