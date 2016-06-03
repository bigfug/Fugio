#include "pinprivate.h"

#include <QSettings>
#include <QDebug>
#include <QStringList>
#include <QDateTime>

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_control_interface.h>
#include <fugio/context_interface.h>
#include <fugio/utils.h>

PinPrivate::PinPrivate( void )
	: mContext( 0 ), mGlobalId( QUuid::createUuid() ), mLocalId( QUuid::createUuid() ), mDirection( PIN_UNKNOWN ),
	  mUpdated( 0 ), mOrder( -1 ), mFlags( Updatable )
{
}

PinPrivate::~PinPrivate( void )
{
	if( mContext != 0 )
	{
		mContext->unregisterPin( mGlobalId );
	}

	mControl.clear();

	//qDebug() << "~PinPrivate" << mName;
}

QString PinPrivate::name( void ) const
{
	return( mName );
}

PinDirection PinPrivate::direction( void ) const
{
	return( mDirection );
}

int PinPrivate::order() const
{
	return( mOrder );
}

void PinPrivate::setOrder( int pOrder )
{
	mOrder = pOrder;
}

bool PinPrivate::removable( void ) const
{
	return( mFlags.testFlag( Removable ) );
}

void PinPrivate::setRemovable( bool pRemovable )
{
	if( pRemovable )
	{
		mFlags |= Removable;
	}
	else
	{
		mFlags &= ~Removable;
	}
}

bool PinPrivate::hidden( void ) const
{
	return( mFlags.testFlag( Hidden ) );
}

void PinPrivate::setHidden( bool pHidden )
{
	if( pHidden )
	{
		mFlags |= Hidden;
	}
	else
	{
		mFlags &= ~Hidden;
	}
}

bool PinPrivate::updatable() const
{
	return( mFlags.testFlag( Updatable ) );
}

void PinPrivate::setUpdatable( bool pUpdatable )
{
	if( pUpdatable )
	{
		mFlags |= Updatable;
	}
	else
	{
		mFlags &= ~Updatable;
	}
}

QList< QSharedPointer<fugio::PinInterface> > PinPrivate::connectedPins( void ) const
{
	return( mContext->connections( mGlobalId ) );
}

void PinPrivate::setSetting( const QString &pKey, const QVariant &pValue )
{
	mSettings.insert( pKey, pValue );
}

QVariant PinPrivate::setting( const QString &pKey, const QVariant &pDefault ) const
{
	return( mSettings.value( pKey, pDefault ) );
}

void PinPrivate::registerInterface(const QUuid &pUuid, QObject *pObject)
{
	mInterfaces.insert( pUuid, pObject );
}

void PinPrivate::unregisterInterface( const QUuid &pUuid )
{
	mInterfaces.remove( pUuid );
}

QObject *PinPrivate::findInterface(const QUuid &pUuid) const
{
	return( mInterfaces.value( pUuid, nullptr ) );
}

qint64 PinPrivate::updated( void ) const
{
	return( mUpdated );
}

bool PinPrivate::isConnected( void ) const
{
	return( mContext->isConnected( mGlobalId ) );
}

bool PinPrivate::isConnectedToActiveNode() const
{
	QList< QSharedPointer<fugio::PinInterface>	>	CP = connectedPins();

	for( QList< QSharedPointer<fugio::PinInterface> >::iterator it = CP.begin() ; it != CP.end() ; it++ )
	{
		PinPrivate		*DstPin = qobject_cast<PinPrivate *>( (*it)->qobject() );

		if( DstPin == 0 || DstPin->node() == 0 || !DstPin->node()->isActive() )
		{
			continue;
		}

		return( true );
	}

	return( false );
}

bool PinPrivate::isUpdated( qint64 pTimeStamp ) const
{
	return( mUpdated >= pTimeStamp );
}

QSharedPointer<fugio::PinControlInterface> PinPrivate::control( void )
{
	return( mControl );
}

QSharedPointer<fugio::PinControlInterface> PinPrivate::control() const
{
	return( mControl );
}

void PinPrivate::setControl( QSharedPointer<fugio::PinControlInterface> pControl )
{
//	if( mControl != 0 )
//	{
//		mControl->object()->setParent( 0 );

//		mControl.clear();
//	}

	mControl = pControl;

//	if( mControl != 0 )
//	{
//		mControl->object()->setParent( this );
	//	}
}

bool PinPrivate::hasControl( void ) const
{
	return( mControl.isNull() ? false : true );
}

void PinPrivate::update( qint64 pTimeStamp, bool pUpdatedConnectedNode )
{
	mUpdated = pTimeStamp;

	if( mDirection == PIN_OUTPUT )
	{
		QList< QSharedPointer<fugio::PinInterface>	>	CP = connectedPins();

		for( QList< QSharedPointer<fugio::PinInterface> >::iterator it = CP.begin() ; it != CP.end() ; it++ )
		{
			PinPrivate		*DstPin = qobject_cast<PinPrivate *>( (*it)->qobject() );

			if( DstPin == 0 || !DstPin->updatable() )
			{
				continue;
			}

			if( DstPin->mUpdated >= pTimeStamp )
			{
				//continue;
			}

			DstPin->mUpdated = qMax( pTimeStamp, DstPin->mUpdated );

			if( pUpdatedConnectedNode )
			{
				mContext->updateNode( DstPin->node() );
			}
		}
	}
	else if( mDirection == PIN_INPUT )
	{
		mContext->updateNode( node() );
	}
}

void PinPrivate::setValue( const QVariant &pVariant )
{
	if( mDefaultValue != pVariant )
	{
		mDefaultValue = pVariant;

		mUpdated = mContext->global()->timestamp();

		emit valueChanged( mDefaultValue );
	}
}

QVariant PinPrivate::value() const
{
	return( mDefaultValue );
}

void PinPrivate::loadSettings1( QSettings &pSettings, bool pPartial )
{
	pSettings.beginGroup( name() );

	//-------------------------------------------------------------------------

	int		SettingsVersion = pSettings.value( "version", 0 ).toInt();

	setName( pSettings.value( "name", mName ).toString() );

	if( !pPartial )
	{
		setGlobalId( QUuid( pSettings.value( "uuid", mGlobalId.toString() ).toString() ) );
	}

	setDirection( PinDirection( pSettings.value( "direction", int( direction() ) ).toInt() ) );

	setOrder( pSettings.value( "order", -1 ).toInt() );

	setRemovable( pSettings.value( "removable", removable() ).toBool() );

	setHidden( pSettings.value( "hidden", hidden() ).toBool() );

	if( direction() == PIN_INPUT )
	{
		setUpdatable( pSettings.value( "updatable", updatable() ).toBool() );
	}

	//-------------------------------------------------------------------------

	if( mControl != 0 )
	{
		if( SettingsVersion >= 1 )
		{
			pSettings.beginGroup( "control" );
		}

		mControl->loadSettings( pSettings );

		if( SettingsVersion >= 1 )
		{
			pSettings.endGroup();
		}
	}

	//-------------------------------------------------------------------------

	pSettings.beginGroup( "settings" );

	foreach( const QString &K, pSettings.childKeys() )
	{
		mSettings.insert( K, pSettings.value( K ) );
	}

	pSettings.endGroup();

	//-------------------------------------------------------------------------

	pSettings.endGroup();
}

void PinPrivate::loadSettings2( QSettings &pSettings, bool pPartial )
{
	Q_UNUSED( pPartial )

	setName( pSettings.value( "name", mName ).toString() );

	setOrder( pSettings.value( "order", -1 ).toInt() );

	setRemovable( pSettings.value( "removable", removable() ).toBool() );

	setHidden( pSettings.value( "hidden", hidden() ).toBool() );

	setValue( pSettings.value( "default", mDefaultValue ) );

	setLocalId( fugio::utils::string2uuid( pSettings.value( "uuid", fugio::utils::uuid2string( localId() ) ).toString() ) );

	setPairedUuid( pSettings.value( "paired", fugio::utils::uuid2string( pairedUuid() ) ).toString() );

	if( direction() == PIN_INPUT )
	{
		setUpdatable( pSettings.value( "updatable", updatable() ).toBool() );
	}

	//-------------------------------------------------------------------------

	if( mControl != 0 )
	{
		pSettings.beginGroup( "control" );

		mControl->loadSettings( pSettings );

		pSettings.endGroup();
	}
}

void PinPrivate::saveSettings1( QSettings &pSettings )
{
	pSettings.beginGroup( name() );

	//-------------------------------------------------------------------------

	pSettings.setValue( "version", 1 );

	pSettings.setValue( "name", mName );
	pSettings.setValue( "uuid", mGlobalId.toString() );
	pSettings.setValue( "direction", int( direction() ) );
	pSettings.setValue( "order", order() );

	if( removable() )
	{
		pSettings.setValue( "removable", removable() );
	}

	if( hidden() )
	{
		pSettings.setValue( "hidden", hidden() );
	}

	if( direction() == PIN_INPUT )
	{
		pSettings.setValue( "updatable", updatable() );
	}

	//-------------------------------------------------------------------------

	if( mControl != 0 )
	{
		pSettings.beginGroup( "control" );

		mControl->saveSettings( pSettings );

		pSettings.endGroup();
	}

	//-------------------------------------------------------------------------

	pSettings.beginGroup( "settings" );

	for( QVariantHash::const_iterator it = mSettings.constBegin() ; it != mSettings.constEnd() ; it++ )
	{
		pSettings.setValue( it.key(), it.value() );
	}

	pSettings.endGroup();

	//-------------------------------------------------------------------------

	pSettings.endGroup();
}

void PinPrivate::saveSettings2( QSettings &pSettings )
{
	pSettings.setValue( "version", 2 );

	pSettings.setValue( "name", mName );
	pSettings.setValue( "direction", int( direction() ) );
	pSettings.setValue( "order", order() );
	pSettings.setValue( "uuid", fugio::utils::uuid2string( localId() ) );

	if( !mPairedId.isNull() )
	{
		pSettings.setValue( "paired", fugio::utils::uuid2string( mPairedId ) );
	}

	if( mDefaultValue.isValid() )
	{
		pSettings.setValue( "default", mDefaultValue );
	}

	if( removable() )
	{
		pSettings.setValue( "removable", removable() );
	}

	if( hidden() )
	{
		pSettings.setValue( "hidden", hidden() );
	}

	if( direction() == PIN_INPUT && !updatable() )
	{
		pSettings.setValue( "updatable", updatable() );
	}

	//-------------------------------------------------------------------------

	if( mControl )
	{
		pSettings.beginGroup( "control" );

		mControl->saveSettings( pSettings );

		pSettings.endGroup();
	}

	//-------------------------------------------------------------------------

	pSettings.beginGroup( "settings" );

	for( QVariantHash::const_iterator it = mSettings.constBegin() ; it != mSettings.constEnd() ; it++ )
	{
		pSettings.setValue( it.key(), it.value() );
	}

	pSettings.endGroup();
}

QSharedPointer<fugio::NodeInterface> PinPrivate::node( void )
{
	return( mNode.toStrongRef() );
}

QSharedPointer<fugio::NodeInterface> PinPrivate::connectedNode( void )
{
	QSharedPointer<fugio::PinInterface>		ConnectedPin = connectedPin();

	if( ConnectedPin.isNull() )
	{
		return( QSharedPointer<fugio::NodeInterface>() );
	}

	return( ConnectedPin->node() );
}

QSharedPointer<fugio::PinInterface> PinPrivate::connectedPin() const
{
	//Q_ASSERT( direction() == PIN_INPUT );

	QList< QSharedPointer<fugio::PinInterface> >	PinLst = mContext->connections( globalId() );

	return( PinLst.empty() ? QSharedPointer<fugio::PinInterface>() : PinLst.first() );
}

void PinPrivate::setContext( fugio::ContextInterface *pContext )
{
	mContext = pContext;
}

void PinPrivate::setGlobalId( const QUuid &pUUID )
{
	if( mGlobalId == pUUID )
	{
		return;
	}

	mContext->renamePin( mGlobalId, pUUID );

	emit renamed( mGlobalId, pUUID );

	mGlobalId = pUUID;
}

void PinPrivate::setLocalId( const QUuid &pUUID )
{
	mLocalId = pUUID;
}

void PinPrivate::setName( const QString &pName )
{
	if( pName != mName )
	{
		mName = pName;

		emit nameChanged( mName );

		emit PinSignals::nameChanged( mContext->findPin( globalId() ) );
	}
}

void PinPrivate::registerPinInputTypes( const QList<QUuid> &pTypeList )
{
	mInputTypeList.append( pTypeList );
}

void PinPrivate::registerPinInputType( const QUuid &pType )
{
	mInputTypeList.append( pType );
}

void PinPrivate::setDirection( PinDirection pDirection )
{
	mDirection = pDirection;
}

void PinPrivate::setNode( QSharedPointer<fugio::NodeInterface> pNode )
{
	mNode = pNode;
}
