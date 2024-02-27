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

	virtual ~NodePrivate( void ) Q_DECL_OVERRIDE;

	void setSettings( const QVariantHash &pSettings )
	{
		mSettings = pSettings;
	}

	void setControl( QSharedPointer<fugio::NodeControlInterface> pControl )
	{
		mControl = pControl;

		if( mContext )
		{
			emit controlChanged( mContext->findNode( mUUID ) );
		}
	}

	void setUuid( const QUuid &pUUID )
	{
		if( mUUID == pUUID )
		{
			return;
		}

		if( mContext )
		{
			mContext->renameNode( mUUID, pUUID );
		}

		mUUID = pUUID;
	}

	void setControlUuid( const QUuid &pUUID )
	{
		mControlUUID = pUUID;
	}

	//-------------------------------------------------------------------------
	// fugio::NodeInterface

	virtual fugio::NodeSignals *qobject( void ) Q_DECL_OVERRIDE
	{
		return( this );
	}

	virtual QUuid uuid( void ) Q_DECL_OVERRIDE
	{
		return( mUUID );
	}

	virtual fugio::ContextInterface *context( void ) Q_DECL_OVERRIDE
	{
		return( mContext );
	}

	virtual void setContext( fugio::ContextInterface *pContext ) Q_DECL_OVERRIDE;

	virtual QSharedPointer<fugio::NodeControlInterface> control( void ) const Q_DECL_OVERRIDE
	{
		return( mControl );
	}

	virtual QString name( void ) const Q_DECL_OVERRIDE
	{
		return( mName );
	}

	virtual void setName( const QString &pName ) Q_DECL_OVERRIDE
	{
		if( pName == mName )
		{
			return;
		}

		mName = pName;

		if( mContext )
		{
			emit nameChanged( mContext->findNode( mUUID ) );
		}

		emit nameChanged( mName );
	}

	virtual QSharedPointer<fugio::PinInterface> findPinByGlobalId( const QUuid &pUuid ) const Q_DECL_OVERRIDE
	{
		auto	it = mPinMap.find( pUuid );

		return( it == mPinMap.end() ? QSharedPointer<fugio::PinInterface>() : it.value() );
	}

	virtual QSharedPointer<fugio::PinInterface> findPinByLocalId( const QUuid &pUuid ) const Q_DECL_OVERRIDE
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

	virtual QSharedPointer<fugio::PinInterface> findPinByName( const QString &pName ) const Q_DECL_OVERRIDE
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

	virtual QSharedPointer<fugio::PinInterface> findInputPinByName( const QString &pName ) const Q_DECL_OVERRIDE
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

	virtual QSharedPointer<fugio::PinInterface> findOutputPinByName( const QString &pName ) const Q_DECL_OVERRIDE
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

	virtual bool hasPin( const QString &pName ) const
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

	virtual QList< QSharedPointer<fugio::PinInterface> > enumPins( void ) const Q_DECL_OVERRIDE
	{
		return( mPinMap.values() );
	}

	virtual QList< QSharedPointer<fugio::PinInterface> > enumInputPins( void ) const Q_DECL_OVERRIDE
	{
		return( mPinInputs );
	}

	virtual QList< QSharedPointer<fugio::PinInterface> > enumOutputPins( void ) const Q_DECL_OVERRIDE
	{
		return( mPinOutputs );
	}

	virtual bool hasPin( QSharedPointer<fugio::PinInterface> pPin ) const Q_DECL_OVERRIDE
	{
		return( mPinMap.key( pPin ).isNull() ? false : true );
	}

	virtual QSharedPointer<fugio::PinInterface> createPin( const QString &pName, PinDirection pDirection, const QUuid &pGlobalId, const QUuid &pLocalId ) Q_DECL_OVERRIDE;

	virtual QObject *createPin( const QString &pName, PinDirection pDirection, const QUuid &pGlobalId, const QUuid &pLocalId, QSharedPointer<fugio::PinInterface> &mPinInterface, const QUuid &pControlUUID ) Q_DECL_OVERRIDE;

	virtual QSharedPointer<fugio::PinInterface> createProperty( const QString &pName, PinDirection pDirection, const QUuid &pGlobalId, const QUuid &pLocalId ) Q_DECL_OVERRIDE;

	virtual QObject *createProperty( const QString &pName, PinDirection pDirection, const QUuid &pGlobalId, const QUuid &pLocalId, QSharedPointer<fugio::PinInterface> &mPinInterface, const QUuid &pControlUUID ) Q_DECL_OVERRIDE;

	virtual void addPin( QSharedPointer<fugio::PinInterface> pPin ) Q_DECL_OVERRIDE;

	virtual void removePin( QSharedPointer<fugio::PinInterface> pPin ) Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp );

	virtual void loadSettings( QSettings &pSettings, QMap<QUuid,QUuid> &pPinMap, bool pPartial ) Q_DECL_OVERRIDE;

	virtual void saveSettings( QSettings &pSettings, bool pPartial ) const Q_DECL_OVERRIDE;

	virtual void updateOutputs( void );

	virtual void setSetting( const QString &pKey, const QVariant &pValue ) Q_DECL_OVERRIDE;

	virtual QVariant setting( const QString &pKey, const QVariant &pDefault = QVariant() ) const Q_DECL_OVERRIDE;

	virtual void update( void );

	virtual bool hasControl( void ) const Q_DECL_OVERRIDE
	{
		return( !mControl.isNull() );
	}

	virtual bool isInitialised( void ) const Q_DECL_OVERRIDE
	{
		return( mStatus == Initialised );
	}

	virtual bool isActive( void ) const Q_DECL_OVERRIDE
	{
		return( mActive );
	}

	virtual void setActive( bool pActive ) Q_DECL_OVERRIDE;

	virtual QVariantHash settings( void ) const Q_DECL_OVERRIDE
	{
		return( mSettings );
	}

	virtual QUuid controlUuid( void ) const Q_DECL_OVERRIDE
	{
		return( mControlUUID );
	}

	virtual QList<UuidPair> pairedPins( void ) Q_DECL_OVERRIDE;

	virtual void pairPins( QSharedPointer<fugio::PinInterface> pPin1, QSharedPointer<fugio::PinInterface> pPin2 ) Q_DECL_OVERRIDE;

	virtual void pairPins( const QUuid &pLocalId1, const QUuid &pLocalId2 ) Q_DECL_OVERRIDE;

	virtual void unpairPins( QSharedPointer<fugio::PinInterface> pPin1, QSharedPointer<fugio::PinInterface> pPin2 ) Q_DECL_OVERRIDE;

	virtual void unpairPins( const QUuid &pLocalId1, const QUuid &pLocalId2 ) Q_DECL_OVERRIDE;

	//-------------------------------------------------------------------------
	// Status

	virtual Status status( void ) const Q_DECL_OVERRIDE
	{
		return( mStatus );
	}

	virtual void setStatus( Status pStatus ) Q_DECL_OVERRIDE
	{
		if( mStatus != pStatus )
		{
			mStatus = pStatus;

			emit statusUpdated();
		}
	}

	virtual QString statusMessage( void ) const Q_DECL_OVERRIDE
	{
		return( mStatusMessage );
	}

	virtual void setStatusMessage( const QString &pMessage ) Q_DECL_OVERRIDE
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
	void loadPins( QSettings &pSettings, const QString &pArrayName, PinDirection pDirection, QMap<QUuid, QUuid> &pPinMap, bool pPartial );

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
