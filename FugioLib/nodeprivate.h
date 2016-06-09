#ifndef NODE_PRIVATE_H
#define NODE_PRIVATE_H

#include <QObject>
#include <QHash>
#include <QVariant>
#include <QSharedPointer>

#include <QDebug>

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/context_interface.h>
#include <fugio/node_signals.h>

class NodePrivate : public fugio::NodeSignals, public fugio::NodeInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::NodeInterface )

public:
	explicit NodePrivate( QSharedPointer<fugio::NodeControlInterface> pControl = QSharedPointer<fugio::NodeControlInterface>() );

	virtual ~NodePrivate( void );

	void setSettings( const QVariantHash &pSettings )
	{
		mSettings = pSettings;
	}

	void setControl( QSharedPointer<fugio::NodeControlInterface> pControl )
	{
		mControl = pControl;

		emit controlChanged( mContext->findNode( mUUID ) );
	}

	void setUuid( const QUuid &pUUID )
	{
		if( mUUID == pUUID )
		{
			return;
		}

		mContext->renameNode( mUUID, pUUID );

		mUUID = pUUID;
	}

	void setContext( fugio::ContextInterface *pContext )
	{
		mContext = pContext;
	}

	void setControlUuid( const QUuid &pUUID )
	{
		mControlUUID = pUUID;
	}

	//-------------------------------------------------------------------------
	// fugio::NodeInterface

	virtual fugio::NodeSignals *qobject( void )
	{
		return( this );
	}

	virtual QUuid uuid( void )
	{
		return( mUUID );
	}

	virtual fugio::ContextInterface *context( void )
	{
		return( mContext );
	}

	virtual QSharedPointer<fugio::NodeControlInterface> control( void )
	{
		return( mControl );
	}

	virtual const QString &name( void )
	{
		return( mName );
	}

	virtual void setName( const QString &pName )
	{
		if( pName == mName )
		{
			return;
		}

		mName = pName;

		emit nameChanged( mContext->findNode( mUUID ) );

		emit nameChanged( mName );
	}

	virtual QSharedPointer<fugio::PinInterface> findPinByGlobalId( const QUuid &pUuid )
	{
		auto	it = mPinMap.find( pUuid );

		return( it == mPinMap.end() ? QSharedPointer<fugio::PinInterface>() : it.value() );
	}

	virtual QSharedPointer<fugio::PinInterface> findPinByLocalId( const QUuid &pUuid )
	{
		for( auto it = mPinMap.begin() ; it != mPinMap.end() ; it++ )
		{
			if( it.value()->localId() == pUuid )
			{
				return( it.value() );
			}
		}

		return( QSharedPointer<fugio::PinInterface>() );
	}

	virtual QSharedPointer<fugio::PinInterface> findPinByName( const QString &pName ) const
	{
		for( auto it = mPinMap.begin() ; it != mPinMap.end() ; it++ )
		{
			if( it.value()->name() == pName )
			{
				return( it.value() );
			}
		}

		return( QSharedPointer<fugio::PinInterface>() );
	}

	virtual QSharedPointer<fugio::PinInterface> findInputPinByName( const QString &pName ) const
	{
		for( auto it = mPinMap.begin() ; it != mPinMap.end() ; it++ )
		{
			if( it.value()->direction() == PIN_INPUT && it.value()->name() == pName )
			{
				return( it.value() );
			}
		}

		return( QSharedPointer<fugio::PinInterface>() );
	}

	virtual QSharedPointer<fugio::PinInterface> findOutputPinByName( const QString &pName ) const
	{
		for( auto it = mPinMap.begin() ; it != mPinMap.end() ; it++ )
		{
			if( it.value()->direction() == PIN_OUTPUT && it.value()->name() == pName )
			{
				return( it.value() );
			}
		}

		return( QSharedPointer<fugio::PinInterface>() );
	}

	bool hasPin( const QString &pName ) const
	{
		for( auto it = mPinMap.begin() ; it != mPinMap.end() ; it++ )
		{
			if( it.value()->name() == pName )
			{
				return( true );
			}
		}

		return( false );
	}

	virtual QList< QSharedPointer<fugio::PinInterface> > enumPins( void )
	{
		return( mPinMap.values() );
	}

	virtual QList< QSharedPointer<fugio::PinInterface> > enumInputPins( void )
	{
		return( mPinInputs );
	}

	virtual QList< QSharedPointer<fugio::PinInterface> > enumOutputPins( void )
	{
		return( mPinOutputs );
	}

	virtual bool hasPin( QSharedPointer<fugio::PinInterface> pPin ) const
	{
		return( mPinMap.key( pPin ).isNull() ? false : true );
	}

	virtual QSharedPointer<fugio::PinInterface> createPin( const QString &pName, PinDirection pDirection, const QUuid &pUuid );

	virtual QObject *createPin( const QString &pName, PinDirection pDirection, const QUuid &pUuid, QSharedPointer<fugio::PinInterface> &mPinInterface, const QUuid &pControlUUID );

	virtual void addPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual void removePin( QSharedPointer<fugio::PinInterface> pPin );

	virtual void inputsUpdated( qint64 pTimeStamp );

	virtual void loadSettings1( QSettings &pSettings, bool pPartial );
	virtual void loadSettings2( QSettings &pSettings, QMap<QUuid,QUuid> &pPinMap, bool pPartial );

	virtual void saveSettings1( QSettings &pSettings, bool pPartial );
	virtual void saveSettings2( QSettings &pSettings, bool pPartial );

	virtual void updateOutputs( void );

	virtual void setSetting( const QString &pKey, const QVariant &pValue );

	virtual QVariant setting( const QString &pKey, const QVariant &pDefault = QVariant() );

	virtual void update( void );

	virtual bool hasControl( void ) const
	{
		return( mControl );
	}

	virtual bool isInitialised( void ) const
	{
		return( mStatus == Initialised );
	}

	virtual bool isActive( void ) const
	{
		return( mActive );
	}

	virtual void setActive( bool pActive );

	virtual QVariantHash settings( void ) const
	{
		return( mSettings );
	}

	virtual QUuid controlUuid( void ) const
	{
		return( mControlUUID );
	}

	virtual QList<UuidPair> pairedPins( void );

	virtual void pairPins( QSharedPointer<fugio::PinInterface> pPin1, QSharedPointer<fugio::PinInterface> pPin2 );

	virtual void pairPins( const QUuid &pLocalId1, const QUuid &pLocalId2 );

	virtual void unpairPins( QSharedPointer<fugio::PinInterface> pPin1, QSharedPointer<fugio::PinInterface> pPin2 );

	virtual void unpairPins( const QUuid &pLocalId1, const QUuid &pLocalId2 );

	//-------------------------------------------------------------------------
	// Status

	virtual Status status( void ) const
	{
		return( mStatus );
	}

	virtual void setStatus( Status pStatus )
	{
		if( mStatus != pStatus )
		{
			mStatus = pStatus;

			emit statusUpdated();
		}
	}

	virtual QString statusMessage( void ) const
	{
		return( mStatusMessage );
	}

	virtual void setStatusMessage( const QString &pMessage )
	{
		if( mStatusMessage != pMessage )
		{
			mStatusMessage = pMessage;

			emit statusUpdated();
		}
	}

	//-------------------------------------------------------------------------

	void clear();

private:
	void loadPins2( QSettings &pSettings, const QString &pArrayName, PinDirection pDirection, QMap<QUuid, QUuid> &pPinMap, bool pPartial );

	void loadPinSettings(QSettings &pSettings, QVariantHash &pVarHsh, QStringList &pVarBse) const;

private slots:
	void pinRenamed( const QUuid &pOld, const QUuid &pNew );

private:
	static bool pinOrderFunction( const QSharedPointer<fugio::PinInterface> &p1, const QSharedPointer<fugio::PinInterface> &p2 );

private:
	QHash< QUuid, QSharedPointer<fugio::PinInterface> >	 mPinMap;
	QList< QSharedPointer<fugio::PinInterface> >		 mPinInputs;
	QList< QSharedPointer<fugio::PinInterface> >		 mPinOutputs;
	QSharedPointer<fugio::NodeControlInterface>			 mControl;
	QUuid												 mUUID;
	QUuid												 mControlUUID;
	QString												 mName;
	QVariantHash										 mSettings;
	fugio::ContextInterface								*mContext;
	bool												 mActive;
	fugio::NodeInterface::Status						 mStatus;
	QString												 mStatusMessage;
};

#endif // NODE_PRIVATE_H
