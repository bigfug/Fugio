#ifndef PJLINKSERVER_H
#define PJLINKSERVER_H

#include <QObject>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QSize>
#include <QTimer>

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
	PJLinkSetting( T pDefault = T() )
		: mCurrentSetting( pDefault ), mNextSetting( pDefault ), mUnknown( true )
	{

	}

	bool update( void )
	{
		mUnknown = false;

		if( mCurrentSetting != mNextSetting )
		{
			mCurrentSetting = mNextSetting;

			return( true );
		}

		return( false );
	}

	bool set( T pValue )
	{
		if( isUnknown() || mCurrentSetting != pValue )
		{
			mCurrentSetting = mNextSetting = pValue;

			mUnknown = false;

			return( true );
		}

		return( false );
	}

	inline bool isUnknown( void ) const
	{
		return( mUnknown );
	}

	inline T current( void ) const
	{
		return( mCurrentSetting );
	}

private:
	bool	mUnknown;
	T		mCurrentSetting;
	T		mNextSetting;
};

typedef struct PJLinkLamp
{
	int		mUsageTime;
	bool	mLampOn;
} PJLinkLamp;

class PJLinkClient : public QObject
{
	Q_OBJECT

public:
	typedef enum Error
	{
		ERROR_UNKNOWN = -1,
		NO_ERROR,
		WARNING,
		ERROR
	} Error;

	typedef enum Power
	{
		POWER_UNKNOWN = -1,
		POWER_OFF,
		POWER_ON,
		COOLING,
		WARM_UP
	} Power;

	typedef enum Input
	{
		INPUT_UNKNOWN = -1,
		RGB = 1,
		VIDEO,
		DIGITAL,
		STORAGE,
		NETWORK,
		INTERNAL
	} Input;

	typedef struct PJLinkInput
	{
		QString		mName;
		Input		mInput;
		char		mSource;
	} PJLinkInput;

public:
	PJLinkClient( QHostAddress pAddress, QObject *pParent = Q_NULLPTR );

	QHostAddress address( void ) const
	{
		return( mAddress );
	}

	static Error errorFromChar( char c )
	{
		switch( c )
		{
			case '0': return( NO_ERROR );
			case '1': return( WARNING );
			case '2': return( ERROR );
		}

		return( ERROR_UNKNOWN );
	}

	static Power powerFromChar( char c )
	{
		switch( c )
		{
			case '0': return( POWER_OFF );
			case '1': return( POWER_ON );
			case '2': return( COOLING );
			case '3': return( WARM_UP );
		}

		return( POWER_UNKNOWN );
	}

	static Input inputFromChar( char c )
	{
		switch( c )
		{
			case '1': return( RGB );
			case '2': return( VIDEO );
			case '3': return( DIGITAL );
			case '4': return( STORAGE );
			case '5': return( NETWORK );
			case '6': return( INTERNAL );
		}

		return( INPUT_UNKNOWN );
	}

	static QSize sizeFromResponse( QByteArray pResponse )
	{
		QByteArrayList	ResLst = pResponse.split( 'x' );

		if( ResLst.size() == 2 )
		{
			return( QSize( ResLst[ 0 ].toInt(), ResLst[ 1 ].toInt() ) );
		}

		return( QSize() );
	}

	QString name( void ) const
	{
		return( mName.current() );
	}

	QString manufacturer( void ) const
	{
		return( mManufacturer.current() );
	}

signals:
	void clientConnected( void );

	void clientUpdated( void );

	void authenticationError( void );

public slots:
	void setPassword( QByteArray pPassword )
	{
		if( mPassword != pPassword )
		{
			mPassword = pPassword;

			mSocket.disconnectFromHost();

			connectToClient();
		}
	}

	void connectToClient( void );

	void powerOn( void );

	void powerOff( void );

	void freeze( bool pFreeze );

	void switchInput( Input pInput, char pValue );

	void setName( QString pName )
	{
		if( mName.set( pName ) )
		{
			emit clientUpdated();
		}
	}

	void setPower( Power pPower )
	{
		if( mPower.set( pPower ) )
		{
			emit clientUpdated();
		}
	}

	void setInput( QByteArray pResponse )
	{
		char	Type		= pResponse.at( 0 );		// 1～6
		char	Value		= pResponse.at( 1 );		// 1～Z

		setInput( inputFromChar( Type ), Value );
	}

	void setInput( Input pType, char pValue )
	{
		bool	b1 = false;

		b1 |= mInputType.set( pType );
		b1 |= mInput.set( pValue );

		if( b1 )
		{
			emit clientUpdated();
		}
	}

	void setStatus( QByteArray pStatus )
	{
		setFanStatus( errorFromChar( pStatus.at( 0 ) ) );
		setLampStatus( errorFromChar( pStatus.at( 1 ) ) );
		setTemperatureStatus( errorFromChar( pStatus.at( 2 ) ) );
		setCoverOpenStatus( errorFromChar( pStatus.at( 3 ) ) );
		setFilterStatus( errorFromChar( pStatus.at( 4 ) ) );
		setOtherStatus( errorFromChar( pStatus.at( 5 ) ) );
	}

	void setFanStatus( Error pError )
	{
		if( mFanStatus.set( pError ) )
		{
			emit clientUpdated();
		}
	}

	void setLampStatus( Error pError )
	{
		if( mLampStatus.set( pError ) )
		{
			emit clientUpdated();
		}
	}

	void setTemperatureStatus( Error pError )
	{
		if( mTemperatureStatus.set( pError ) )
		{
			emit clientUpdated();
		}
	}

	void setCoverOpenStatus( Error pError )
	{
		if( mCoverOpenStatus.set( pError ) )
		{
			emit clientUpdated();
		}
	}

	void setFilterStatus( Error pError )
	{
		if( mFilterStatus.set( pError ) )
		{
			emit clientUpdated();
		}
	}

	void setOtherStatus( Error pError )
	{
		if( mOtherStatus.set( pError ) )
		{
			emit clientUpdated();
		}
	}

	void updateStatus( void );

private slots:
	void sendCommand( QByteArray pCommand );

	void sendCommand( void );

	void socketError( QAbstractSocket::SocketError pSocketError );

	void readyRead( void );

	void parseResponse( QByteArray pResponse );

	void queryStatus( void );

public:
	inline Power powerStatus( void ) const
	{
		return( mPower.current() );
	}

	inline QString productName( void ) const
	{
		return( mProductName.current() );
	}

	inline QString information( void ) const
	{
		return( mOtherInformation.current() );
	}

	inline QString serialNumber( void ) const
	{
		return( mSerialNumber.current() );
	}

	inline QString softwareVersion( void ) const
	{
		return( mSoftwareVersion.current() );
	}

	inline QSize inputResolution( void ) const
	{
		return( mInputResolution.current() );
	}

	inline QSize recommendedResolution( void ) const
	{
		return( mRecommendedResolution.current() );
	}

	inline int filterUsageTime( void ) const
	{
		return( mFilterUsageTime.current() );
	}

	inline Input inputType( void ) const
	{
		return( mInputType.current() );
	}

	inline char input( void ) const
	{
		return( mInput.current() );
	}

	inline bool videoMuted( void ) const
	{
		return( mVideoMute.current() );
	}

	inline bool audioMuted( void ) const
	{
		return( mAudioMute.current() );
	}

	inline bool frozen( void ) const
	{
		return( mFreeze.current() );
	}

	inline Error fanStatus( void ) const
	{
		return( mFanStatus.current() );
	}

	inline Error lampStatus( void ) const
	{
		return( mFanStatus.current() );
	}

	inline Error temperatureStatus( void ) const
	{
		return( mTemperatureStatus.current() );
	}

	inline Error coverOpenStatus( void ) const
	{
		return( mCoverOpenStatus.current() );
	}

	inline Error filterStatus( void ) const
	{
		return( mFilterStatus.current() );
	}

	inline Error otherStatus( void ) const
	{
		return( mOtherStatus.current() );
	}

private:
	QTcpSocket						 mSocket;
	QHostAddress					 mAddress;
	bool							 mAuthenticated;
	bool							 mReady;
	QByteArray						 mPassword;
	QByteArrayList					 mCommands;
	QByteArray						 mDigest;

	QVector<PJLinkLamp>				 mLamps;
	QVector<PJLinkInput>			 mInputs;

	PJLinkSetting<int>				 mClass;
	PJLinkSetting<Power>			 mPower;
	PJLinkSetting<QString>			 mName;
	PJLinkSetting<QString>			 mManufacturer;
	PJLinkSetting<QString>			 mProductName;
	PJLinkSetting<QString>			 mOtherInformation;
	PJLinkSetting<QString>			 mSerialNumber;
	PJLinkSetting<QString>			 mSoftwareVersion;
	PJLinkSetting<QSize>			 mInputResolution;
	PJLinkSetting<QSize>			 mRecommendedResolution;
	PJLinkSetting<int>				 mFilterUsageTime;
	PJLinkSetting<QByteArrayList>	 mLampModelNumber;
	PJLinkSetting<QByteArrayList>	 mFilterModelNumber;
	PJLinkSetting<Input>			 mInputType;
	PJLinkSetting<char>				 mInput;
	PJLinkSetting<bool>				 mVideoMute;
	PJLinkSetting<bool>				 mAudioMute;
	PJLinkSetting<Error>			 mFanStatus;
	PJLinkSetting<Error>			 mLampStatus;
	PJLinkSetting<Error>			 mTemperatureStatus;
	PJLinkSetting<Error>			 mCoverOpenStatus;
	PJLinkSetting<Error>			 mFilterStatus;
	PJLinkSetting<Error>			 mOtherStatus;
	PJLinkSetting<bool>				 mFreeze;
	int								 mInputTerminalNameIndex;
};

class PJLinkServer : public QObject
{
	Q_OBJECT

public:
	PJLinkServer( QObject *pParent = Q_NULLPTR );

	virtual ~PJLinkServer( void ) Q_DECL_OVERRIDE;

	QVector<QHostAddress> clientAddresses( void ) const;

	PJLinkClient *client( QHostAddress pAddress );

signals:
	void clientQueryStatus( void );

	void clientListChanged( void );

	void clientUpdated( PJLinkClient *pClient );

	void clientAuthenticationError( PJLinkClient *pClient );

	void clientConnected( PJLinkClient *pClient );

private slots:
	void searchTimeout( void );

	void readReadyIP4( void );

	void readReadyIP6( void );

private:
	PJLinkClient *clientAlloc( QHostAddress pAddress );

	void readSocket( QUdpSocket *pSocket );

private:
	QTimer					*mClientQueryTimer;

	QUdpSocket				*mSocketIP4;
	QUdpSocket				*mSocketIP6;

	QVector<PJLinkClient *>	 mClientList;
};

#endif // PJLINKSERVER_H
