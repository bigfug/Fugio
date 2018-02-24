#ifndef PIN_PRIVATE_H
#define PIN_PRIVATE_H

#include <QObject>
#include <QMap>
#include <QUuid>
#include <QVariant>
#include <QSharedPointer>

#include <fugio/pin_interface.h>
#include <fugio/pin_signals.h>

FUGIO_NAMESPACE_BEGIN
class ContextInterface;
FUGIO_NAMESPACE_END

class PinPrivate : public fugio::PinSignals, public fugio::PinInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::PinInterface )

	Q_PROPERTY( QUuid globalId READ globalId )
	Q_PROPERTY( QString name READ name WRITE setName NOTIFY nameChanged )
	Q_PROPERTY( QVariant value READ value WRITE setValue NOTIFY valueChanged )

public:
	explicit PinPrivate( void );

	virtual ~PinPrivate( void );

	//-------------------------------------------------------------------------
	// fugio::PinInterface

	inline virtual QUuid globalId( void ) const Q_DECL_OVERRIDE
	{
		return( mGlobalId );
	}

	inline virtual QUuid localId( void ) const Q_DECL_OVERRIDE
	{
		return( mLocalId );
	}

	virtual fugio::PinSignals *qobject( void ) Q_DECL_OVERRIDE
	{
		return( this );
	}

	Q_INVOKABLE virtual QString name( void ) const Q_DECL_OVERRIDE;

	virtual PinDirection direction( void ) const Q_DECL_OVERRIDE;

	virtual int order( void ) const Q_DECL_OVERRIDE;

	virtual void setOrder( int pOrder ) Q_DECL_OVERRIDE;

	virtual bool removable( void ) const Q_DECL_OVERRIDE;

	virtual void setRemovable( bool pRemovable ) Q_DECL_OVERRIDE;

	virtual bool hidden( void ) const Q_DECL_OVERRIDE;

	virtual void setHidden( bool pHidden ) Q_DECL_OVERRIDE;

	virtual bool updatable( void ) const Q_DECL_OVERRIDE;

	virtual void setUpdatable( bool pUpdatable ) Q_DECL_OVERRIDE;

	virtual bool autoRename( void ) const Q_DECL_OVERRIDE;

	virtual void setAutoRename( bool pAutoRename ) Q_DECL_OVERRIDE;

	virtual fugio::NodeInterface *node( void ) Q_DECL_OVERRIDE;

	virtual qint64 updated( void ) const Q_DECL_OVERRIDE;

	virtual qint64 updatedGlobal( void ) const Q_DECL_OVERRIDE;

	virtual bool isConnected( void ) const Q_DECL_OVERRIDE;

	virtual bool isConnectedToActiveNode( void ) const Q_DECL_OVERRIDE;

	virtual bool isUpdated( qint64 pTimeStamp ) const Q_DECL_OVERRIDE;

	virtual QSharedPointer<fugio::PinControlInterface> control( void ) Q_DECL_OVERRIDE;

	virtual QSharedPointer<fugio::PinControlInterface> control( void ) const Q_DECL_OVERRIDE;

	virtual void setControl( QSharedPointer<fugio::PinControlInterface> pControl ) Q_DECL_OVERRIDE;

	virtual bool hasControl( void ) const Q_DECL_OVERRIDE;

	virtual void setValue( const QVariant &pVariant ) Q_DECL_OVERRIDE;

	virtual QVariant value( void ) const Q_DECL_OVERRIDE;

	virtual void loadSettings( QSettings &pSettings, bool pPartial ) Q_DECL_OVERRIDE;

	virtual void saveSettings( QSettings &pSettings ) const Q_DECL_OVERRIDE;

	virtual fugio::NodeInterface *connectedNode( void ) Q_DECL_OVERRIDE;

	virtual QSharedPointer<fugio::PinInterface> connectedPin( void ) const Q_DECL_OVERRIDE;

	virtual QList< QSharedPointer<fugio::PinInterface> > connectedPins( void ) const Q_DECL_OVERRIDE;

	virtual void setSetting( const QString &pKey, const QVariant &pValue ) Q_DECL_OVERRIDE;

	virtual void clearSetting( const QString &pKey ) Q_DECL_OVERRIDE;

	virtual QVariant setting( const QString &pKey, const QVariant &pDefault = QVariant() ) const Q_DECL_OVERRIDE;

	virtual void registerInterface( const QUuid &pUuid, QObject *pObject ) Q_DECL_OVERRIDE;
	virtual void unregisterInterface( const QUuid &pUuid ) Q_DECL_OVERRIDE;

	virtual QObject *findInterface( const QUuid &pUuid ) const Q_DECL_OVERRIDE;

	virtual QObjectList interfaces( void ) const Q_DECL_OVERRIDE
	{
		return( mInterfaces.values() );
	}

	virtual QUuid controlUuid( void ) const Q_DECL_OVERRIDE
	{
		return( mControlId );
	}

	Q_INVOKABLE virtual void setName( const QString &pName ) Q_DECL_OVERRIDE;

	virtual void registerPinInputTypes( const QList<QUuid> &pTypeList ) Q_DECL_OVERRIDE;
	virtual void registerPinInputType( const QUuid &pTypeList ) Q_DECL_OVERRIDE;

	virtual QList<QUuid> inputTypes( void ) const Q_DECL_OVERRIDE
	{
		return( mInputTypeList );
	}

	virtual void setDescription( const QString &pDescription ) Q_DECL_OVERRIDE
	{
		mDescription = pDescription;
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( mDescription );
	}

	virtual QUuid pairedUuid( void ) const Q_DECL_OVERRIDE
	{
		return( mPairedId );
	}

	inline virtual void setAlwaysUpdate( bool pAlwaysUpdate = true ) Q_DECL_OVERRIDE
	{
#if( QT_VERSION >= QT_VERSION_CHECK( 5, 7, 0 ) )
		mFlags.setFlag( AlwaysUpdate, pAlwaysUpdate );
#else
		if( pAlwaysUpdate )
		{
			mFlags |= AlwaysUpdate;
		}
		else
		{
			mFlags &= ~AlwaysUpdate;
		}
#endif
	}

	inline virtual bool alwaysUpdate( void ) const Q_DECL_OVERRIDE
	{
		return( mFlags.testFlag( AlwaysUpdate ) );
	}

	virtual void setDisplayLabel(QString pDisplayLabel) Q_DECL_OVERRIDE;
	virtual QString displayLabel() const Q_DECL_OVERRIDE;

	//-------------------------------------------------------------------------

	void setPairedUuid( const QUuid &pUuid )
	{
		mPairedId = pUuid;
	}

	void setControlUuid( const QUuid &pUUID )
	{
		mControlId = pUUID;
	}

	void setUpdated( qint64 pTimeStamp )
	{
		mUpdated = pTimeStamp;
	}

	virtual void setGlobalTimestamp( qint64 pGlobalTimestamp )
	{
		mGlobalUpdated = pGlobalTimestamp;
	}

	void setContext( fugio::ContextInterface *pContext );

	void setGlobalId( const QUuid &pUUID );

	void setLocalId( const QUuid &pUUID );

	void setDirection( PinDirection pDirection );

	void setNode( fugio::NodeInterface *pNode );

	void setSettings( const QVariantHash &pSettings )
	{
		mSettings = pSettings;
	}

	void update( qint64 pTimeStamp, bool pUpdatedConnectedNode = true );

	enum Option {
		Removable		= 1 << 0,
		Hidden			= 1 << 1,
		Updatable		= 1 << 2,
		AutoRename		= 1 << 3,
		AlwaysUpdate	= 1 << 4
	};

	Q_DECLARE_FLAGS( Options, Option )

signals:
	void nameChanged( const QString &pName );
	void valueChanged( const QVariant &pValue );

private:
	fugio::ContextInterface						*mContext;
	fugio::NodeInterface						*mNode;
	QUuid										 mGlobalId;			// Context PinMap - Globally unique
	QUuid										 mLocalId;			// Node
	QUuid										 mControlId;		// PinControl PID_*
	QUuid										 mPairedId;			// PinPair input <-> output
	QString										 mName;
	PinDirection								 mDirection;
	QSharedPointer<fugio::PinControlInterface>	 mControl;
	qint64										 mUpdated;			// Local timestamp
	qint64										 mGlobalUpdated;	// Global timestamp
	QVariant									 mDefaultValue;
	QVariantHash								 mSettings;
	int											 mOrder;
	Options										 mFlags;
	QMap<QUuid,QObject *>						 mInterfaces;
	QList<QUuid>								 mInputTypeList;
	QString										 mDescription;
	QString										 mDisplayLabel;
};

Q_DECLARE_OPERATORS_FOR_FLAGS( PinPrivate::Options )

#endif // PIN_PRIVATE_H
