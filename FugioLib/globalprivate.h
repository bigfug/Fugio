#ifndef GLOBAL_PRIVATE_H
#define GLOBAL_PRIVATE_H

#include <QObject>
#include <QMap>
#include <QDir>
#include <QSharedPointer>
#include <QNetworkReply>
#include <QMultiMap>
#include <QElapsedTimer>
#include <QCommandLineParser>

#include <fugio/global.h>

#include <fugio/global_interface.h>
#include <fugio/context_interface.h>
#include <fugio/device_factory_interface.h>

#include <fugio/global_signals.h>

//class IPlugin;

class FUGIOLIBSHARED_EXPORT GlobalPrivate : public fugio::GlobalSignals, public fugio::GlobalInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::GlobalInterface )

	explicit GlobalPrivate( QObject *pParent = 0 );

public:
	virtual ~GlobalPrivate( void );

    void loadPlugins( QDir pDir );

	void unloadPlugins( void );

	bool loadPlugin( const QString &pFileName );

	void registerPlugin( QObject *pPluginInstance );

	void initialisePlugins( void );

	//-------------------------------------------------------------------------
	// fugio::IGlobal

	virtual fugio::GlobalSignals *qobject( void )
	{
		return( this );
	}

	virtual qint64 timestamp( void ) const
	{
		return( mGlobalTimer.elapsed() );
	}

	//-------------------------------------------------------------------------
	// Pause global execution

	virtual bool isPaused( void ) const
	{
		return( mPause );
	}

	virtual void pause( void )
	{
		mPause = true;
	}

	virtual void unpause( void )
	{
		mPause = false;
	}

	//-------------------------------------------------------------------------

	virtual QCommandLineParser &commandLineParser( void )
	{
		return( mCommandLineParser );
	}

	virtual void registerInterface( const QUuid &pUuid, QObject *pInterface );
	virtual void unregisterInterface( const QUuid &pUuid );

	virtual QObject *findInterface( const QUuid &pUuid );

	virtual void registerNodeClasses( const fugio::ClassEntryList &pNodes );
	virtual void unregisterNodeClasses( const fugio::ClassEntryList &pNodes );

	virtual void registerNodeClasses( const fugio::ClassEntry pNodes[] );
	virtual void unregisterNodeClasses( const fugio::ClassEntry pNodes[] );

	virtual void registerPinClasses( const fugio::ClassEntryList &pNodes );
	virtual void unregisterPinClasses( const fugio::ClassEntryList &pNodes );

	virtual void registerPinClasses( const fugio::ClassEntry pNodes[] );
	virtual void unregisterPinClasses( const fugio::ClassEntry pNodes[] );

	virtual QSharedPointer<fugio::NodeInterface> createNode( fugio::ContextInterface *pContext, const QString &pName, const QUuid &pUuid, const QUuid &pOrigId, const QVariantHash &pSettings );

	virtual QSharedPointer<fugio::PinInterface> createPin(const QString &pName, const QUuid &pLocalId, QSharedPointer<fugio::NodeInterface> pNode, PinDirection pDirection, const QUuid &pControlUUID , const QVariantHash &pSettings );

	virtual QSharedPointer<fugio::PinControlInterface> createPinControl( const QUuid &pUUID , QSharedPointer<fugio::PinInterface> pPin );

	virtual const QMetaObject *findNodeMetaObject( const QString &pClassName ) const;

	virtual fugio::ClassEntry findNodeClassEntry( const QUuid &pNodeUuid ) const;

	virtual const QMetaObject *findNodeMetaObject( const QUuid &pNodeUuid ) const;
	virtual const QMetaObject *findPinMetaObject( const QUuid &pPinUuid ) const;

	virtual QString nodeName( const QUuid &pUuid ) const;
	virtual QString pinName( const QUuid &pUuid ) const;

	virtual QUuid findNodeByClass( const QString &pClassName ) const;
	virtual QUuid findPinByClass( const QString &pClassName ) const;

	virtual QStringList pinNames( void ) const;

	virtual QMap<QUuid, QString> pinIds( void ) const
	{
		return( mPinNameMap );
	}

	virtual bool updatePinControl( QSharedPointer<fugio::PinInterface> pPin, const QUuid &pPinControlUuid );

	virtual void clear( void );

	virtual QSharedPointer<fugio::ContextInterface> newContext( void );

	virtual void delContext( QSharedPointer<fugio::ContextInterface> pContext );

	virtual QList< QSharedPointer<fugio::ContextInterface> > contexts( void );

	virtual void setMainWindow( QMainWindow *pMainWindow );

	virtual QMainWindow *mainWindow( void );

	virtual void setEditTarget( fugio::EditInterface *pEditTarget );

	virtual void registerDeviceFactory( fugio::DeviceFactoryInterface *pFactory );
	virtual void unregisterDeviceFactory( fugio::DeviceFactoryInterface *pFactory );

	virtual QStringList deviceFactoryMenuTextList( void ) const;
	virtual void deviceFactoryGui( QWidget *pParent, const QString &pMenuText );

	virtual void loadConfig( QSettings &pSettings );
	virtual void saveConfig( QSettings &pSettings ) const;

	//-------------------------------------------------------------------------
	// Joining and Splitting

	virtual void registerPinSplitter( const QUuid &pPinId, const QUuid &pNodeId );
	virtual void registerPinJoiner( const QUuid &pPinId, const QUuid &pNodeId );

	virtual QList<QUuid> pinSplitters( const QUuid &pPinId ) const;
	virtual QList<QUuid> pinJoiners( const QUuid &pPinId ) const;

	//-------------------------------------------------------------------------

	QUuid instanceId( void ) const;

	static GlobalPrivate *instance( void );

	QList<QObject *> plugInitList( void )
	{
		return( mPluginInitList );
	}

	QList<QObject *> plugInstList( void )
	{
		return( mPluginInstances );
	}

	typedef QMap<QUuid, const QMetaObject *>		UuidClassMap;
	typedef QMap<QUuid, QString>					UuidNameMap;
	typedef QMap<QUuid, QObject *>					UuidObjectMap;
	typedef QMap<QUuid, fugio::ClassEntry>			UuidClassEntryMap;

	const UuidClassEntryMap &nodeMap( void ) const
	{
		return( mNodeMap );
	}

protected:
	bool registerNodeClass( const fugio::ClassEntry &E );

	void unregisterNodeClass( const QUuid &pUUID );

	bool registerPinClass( const QString &pName, const QUuid &pUUID, const QMetaObject *pMetaObject );

	void unregisterPinClass( const QUuid &pUUID );

signals:
	void nodeClassAdded( const fugio::ClassEntry &pClassEntry );

	void globalStart( qint64 pTimeStamp );
	void globalEnd( qint64 pTimeStamp );

private slots:
	void timeout( void );

private:
	static GlobalPrivate			*mInstance;

	QElapsedTimer					 mGlobalTimer;

	QNetworkAccessManager			*mNetworkManager;

//	UuidClassMap					 mNodeClassMap;
//	UuidNameMap						 mNodeNameMap;
	UuidClassEntryMap				 mNodeMap;
	UuidClassMap					 mPinClassMap;
	UuidNameMap						 mPinNameMap;
	UuidObjectMap					 mInterfaceMap;
	int								 mFrameCount;
	qint64							 mLastTime;
	QMainWindow						*mMainWindow;

	QList<QObject *>									 mPluginInstances;
	QList<QSharedPointer<fugio::ContextInterface> >		 mContexts;
	QList<fugio::DeviceFactoryInterface *>				 mDeviceFactories;

	QList<QObject *>				 mPluginInitList;

	fugio::EditInterface					*mEditTarget;

	QMultiMap<QUuid,QUuid>			 mPinSplitters;
	QMultiMap<QUuid,QUuid>			 mPinJoiners;

	QCommandLineParser				 mCommandLineParser;

	bool							 mPause;
};

#endif // GLOBAL_PRIVATE_H
