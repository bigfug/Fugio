#ifndef PJLINKSERVER_H
#define PJLINKSERVER_H

#include <QObject>
#include <QUdpSocket>
#include <QTcpSocket>

class PJLinkReponse
{
public:
	typedef enum ResponseCode
	{
		OK = 0,
		ERR1, ERR2, ERR3, ERR4,
		PARSE_ERROR
	} ResponseCode;

	PJLinkReponse( QByteArray pResponse )
		: mResponseCode( PARSE_ERROR )
	{
		if( !pResponse.startsWith( '%' ) || !pResponse.endsWith( '\r' ) )
		{
			return;
		}

		pResponse.remove( 0, 1 );
		pResponse.chop( 1 );

		if( pResponse.startsWith( '1' ) )
		{
			mClass = 1;
		}
		else if( pResponse.startsWith( '2' ) )
		{
			mClass = 2;
		}
		else
		{
			return;
		}

		pResponse.remove( 0, 1 );

		QByteArrayList	L = pResponse.split( '=' );

		if( L.size() != 2 )
		{
			return;
		}

		mResponseCode = OK;

		mCommandBody = L[ 0 ].toUpper();
		mResponse    = L[ 1 ];

		if( mResponse == "ERR1" )
		{
			mResponseCode = ERR1;
		}
		else if( mResponse == "ERR2" )
		{
			mResponseCode = ERR2;
		}
		else if( mResponse == "ERR3" )
		{
			mResponseCode = ERR3;
		}
		else if( mResponse == "ERR4" )
		{
			mResponseCode = ERR4;
		}
	}

	QByteArray command( void ) const
	{
		return( mCommandBody );
	}

	QByteArray response( void ) const
	{
		return( mResponse );
	}

	ResponseCode resposeCode( void ) const
	{
		return( mResponseCode );
	}

public:
	int				mClass;
	QByteArray		mCommandBody;
	QByteArray		mResponse;
	ResponseCode	mResponseCode;
};

template <class T> class PJLinkSetting
{
public:
	bool	mUnknown;
	T		mCurrentSetting;
	T		mNextSetting;

	PJLinkSetting( void ) : mUnknown( true )
	{

	}

	void update( void )
	{
		mCurrentSetting = mNextSetting;

		mUnknown = false;
	}

	void set( T pValue )
	{
		mCurrentSetting = mNextSetting = pValue;

		mUnknown = false;
	}
};

class PJLinkClient : public QObject
{
	Q_OBJECT

public:
	PJLinkClient( QHostAddress pAddress );

	QHostAddress address( void ) const
	{
		return( mAddress );
	}

public slots:
	void connectToClient( void );

	void powerOn( void );

	void powerOff( void );

	void setPower( int pPower )
	{
		mPower.set( pPower );
	}

private slots:
	void sendCommand( QByteArray pCommand );

	void sendCommand( void );

	void socketError( QAbstractSocket::SocketError pSocketError );

	void readyRead( void );

	void parseResponse( QByteArray pResponse );

private:
	QTcpSocket		mSocket;
	QHostAddress	mAddress;
	bool			mAuthenticated;
	bool			mReady;
	QByteArrayList	mCommands;
	QByteArray		mDigest;
	int				mClass;

	PJLinkSetting<int>		 mPower;
	PJLinkSetting<QString>	 mName;
};

class PJLinkServer : public QObject
{
	Q_OBJECT

public:
	PJLinkServer( QObject *pParent = Q_NULLPTR );

	virtual ~PJLinkServer( void ) Q_DECL_OVERRIDE;

private slots:
	void searchTimeout( void );

	void readReady( void );

private:
	PJLinkClient *client( QHostAddress pAddress );

private:
	QUdpSocket				*mSocketIP4;

	QList<PJLinkClient *>	 mClientList;
};

#endif // PJLINKSERVER_H
