#include "nodeprivate.h"

#include <QSettings>
#include <QStringList>
#include <QtAlgorithms>

#include <fugio/global_interface.h>
#include <fugio/context_interface.h>

#include <fugio/utils.h>

#include "pinprivate.h"

NodePrivate::NodePrivate( QSharedPointer<fugio::NodeControlInterface> pControl )
	: mControl( pControl ), mUUID( QUuid::createUuid() ), mContext( 0 ), mActive( true ), mStatus( Initialising )
{
}

NodePrivate::~NodePrivate( void )
{
	clear();

	//qDebug() << "~NodePrivate" << mName;
}

void NodePrivate::clear()
{
	mContext->unregisterNode( mUUID );

	mControl.clear();

	mPinMap.clear();

	mPinInputs.clear();

	mPinOutputs.clear();
}

QSharedPointer<fugio::PinInterface> NodePrivate::createPin( const QString &pName, PinDirection pDirection, const QUuid &pUuid )
{
	if( mContext == 0 )
	{
		return( QSharedPointer<fugio::PinInterface>() );
	}

	QSharedPointer<fugio::NodeInterface>	NODE = context()->findNode( uuid() );

	if( NODE == 0 )
	{
		return( QSharedPointer<fugio::PinInterface>() );
	}

	QSharedPointer<fugio::PinInterface>	PIN  = context()->global()->createPin( pName, pUuid, NODE, pDirection );

	if( PIN == 0 )
	{
		return( QSharedPointer<fugio::PinInterface>() );
	}

	addPin( PIN );

	return( PIN );
}

QObject *NodePrivate::createPin(const QString &pName, PinDirection pDirection, const QUuid &pUuid, QSharedPointer<fugio::PinInterface> &pPinInterface, const QUuid &pControlUUID )
{
	if( mContext == 0 )
	{
		return( 0 );
	}

	QSharedPointer<fugio::NodeInterface>	NODE = context()->findNode( uuid() );

	if( NODE == 0 )
	{
		return( 0 );
	}

	pPinInterface = context()->global()->createPin( pName, pUuid, NODE, pDirection, pControlUUID );

	if( pPinInterface == 0 )
	{
		return( 0 );
	}

	addPin( pPinInterface );

	return( pPinInterface->control() ? pPinInterface->control()->qobject() : nullptr );
}

void NodePrivate::addPin( QSharedPointer<fugio::PinInterface> pPin )
{
	Q_ASSERT( !pPin->globalId().isNull() );

#if 0
	if( hasPin( pPin->name() ) )
	{
		qDebug() << pPin->name();
	}

	Q_ASSERT( !hasPin( pPin->name() ) );
#endif

	mPinMap.insert( pPin->globalId(), pPin );

	if( pPin->direction() == PIN_INPUT )
	{
		if( pPin->order() == -1 )
		{
			pPin->setOrder( mPinInputs.size() );
		}

		mPinInputs.append( pPin );
	}
	else if( pPin->direction() == PIN_OUTPUT )
	{
		if( pPin->order() == -1 )
		{
			pPin->setOrder( mPinOutputs.size() );
		}

		mPinOutputs.append( pPin );
	}

	connect( pPin->qobject(), &fugio::PinSignals::renamed, this, &NodePrivate::pinRenamed );

	emit pinAdded( mContext->findNode( mUUID ), pPin );
}

void NodePrivate::removePin(QSharedPointer<fugio::PinInterface> pPin)
{
	disconnect( pPin->qobject() );

	mPinMap.remove( pPin->globalId() );

	if( pPin->direction() == PIN_INPUT )
	{
		mPinInputs.removeAll( pPin );
	}
	else
	{
		mPinOutputs.removeAll( pPin );
	}

	emit pinRemoved( mContext->findNode( mUUID ), pPin );
}

void NodePrivate::inputsUpdated( qint64 pTimeStamp )
{
	if( mControl )
	{
		mControl->inputsUpdated( pTimeStamp );
	}
}

void NodePrivate::loadSettings1( QSettings &pSettings, bool pPartial )
{
	pSettings.beginGroup( name() );

	//-------------------------------------------------------------------------

	int		SettingsVersion = pSettings.value( "version", 0 ).toInt();

	setName( pSettings.value( "name", mName ).toString() );

	if( !pPartial )
	{
		setUuid( QUuid( pSettings.value( "uuid", mUUID.toString() ).toString() ) );
	}

	setActive( pSettings.value( "active", mActive ).toBool() );

	//-------------------------------------------------------------------------

	if( SettingsVersion >= 1 )
	{
		pSettings.beginGroup( "control" );
	}

	if( control() )
	{
		control()->loadSettings( pSettings );
	}

	if( SettingsVersion >= 1 )
	{
		pSettings.endGroup();
	}

	//-------------------------------------------------------------------------

	if( SettingsVersion >= 1 )
	{
		pSettings.beginGroup( "pins" );

		foreach( const QString &PIN_NAME, pSettings.childGroups() )
		{
			if( hasPin( PIN_NAME ) )
			{
				continue;
			}

			pSettings.beginGroup( PIN_NAME );

			QSharedPointer<fugio::PinInterface>	PIN = context()->global()->createPin( PIN_NAME, QUuid::createUuid(), context()->findNode( uuid() ), PinDirection( pSettings.value( "direction", int( PIN_UNKNOWN ) ).toInt() ) );

			if( PIN != 0 )
			{
				addPin( PIN );
			}

			pSettings.endGroup();
		}

		qSort( mPinInputs.begin(), mPinInputs.end(), pinOrderFunction );
		qSort( mPinOutputs.begin(), mPinOutputs.end(), pinOrderFunction );
	}

	// mPinMap might change during loading here

	for( int i = 0 ; i < mPinInputs.size() ; i++ )
	{
		QSharedPointer<fugio::PinInterface> P = mPinInputs.at( i );

		P->loadSettings1( pSettings, pPartial );

		//if( P->order() == -1 )
		{
			P->setOrder( i );
		}
	}

	for( int i = 0 ; i < mPinOutputs.size() ; i++ )
	{
		QSharedPointer<fugio::PinInterface> P = mPinOutputs.at( i );

		P->loadSettings1( pSettings, pPartial );

		//if( P->order() == -1 )
		{
			P->setOrder( i );
		}
	}

	qSort( mPinInputs.begin(), mPinInputs.end(), pinOrderFunction );
	qSort( mPinOutputs.begin(), mPinOutputs.end(), pinOrderFunction );

	if( SettingsVersion >= 1 )
	{
		pSettings.endGroup();
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

void NodePrivate::loadPinSettings( QSettings &pSettings, QVariantHash &pVarHsh, QStringList &pVarBse ) const
{
	foreach( const QString &K, pSettings.childGroups() )
	{
		pSettings.beginGroup( K );

		pVarBse.append( K );

		loadPinSettings( pSettings, pVarHsh, pVarBse );

		pVarBse.removeLast();

		pSettings.endGroup();
	}

	foreach( const QString &K, pSettings.childKeys() )
	{
		pVarBse.append( K );

		pVarHsh.insert( pVarBse.join( "/" ), pSettings.value( K ) );

		pVarBse.removeLast();
	}
}

void NodePrivate::loadPins2( QSettings &pSettings, const QString &pArrayName, PinDirection pDirection, QMap<QUuid, QUuid> &pPinMap, bool pPartial )
{
	QSettings		CFG( pSettings.fileName(), pSettings.format() );

	const int		PinCnt = pSettings.beginReadArray( pArrayName );

	for( int i = 0 ; i < PinCnt ; i++ )
	{
		pSettings.setArrayIndex( i );

		const QString	PinGrp = pSettings.childKeys().first();
		const QString	PinCtl = pSettings.value( PinGrp ).toString();

		if( !CFG.childGroups().contains( PinGrp ) )
		{
			continue;
		}

		CFG.beginGroup( PinGrp );

		const QUuid		PinGlobalId = fugio::utils::string2uuid( PinGrp );
		const QUuid		PinLocalId  = CFG.value( "uuid" ).value<QUuid>();

		QVariantHash			PinData;

		if( CFG.childGroups().contains( "settings" ) )
		{
			CFG.beginGroup( "settings" );

			QStringList		TmpLst;

			loadPinSettings( CFG, PinData, TmpLst );

			CFG.endGroup();
		}

		QSharedPointer<fugio::PinInterface>	PIN = findPinByLocalId( PinLocalId );

		if( PIN )
		{
			PinPrivate *PP = qobject_cast<PinPrivate *>( PIN->qobject() );

			if( !pPartial )
			{
				PP->setGlobalId( PinGlobalId );
			}
			else
			{
				//qDebug() << "PIN PARTIAL:" << PP->uuid() << PinId;
			}

			pPinMap.insert( PinGlobalId, PP->globalId() );

			PP->setSettings( PinData );

			PIN->loadSettings2( CFG, pPartial );

			PIN->setOrder( i );
		}
		else
		{
			const QString			PinName = CFG.value( "name" ).toString();

			QUuid	TmpId = pPartial ? QUuid::createUuid() : PinGlobalId;

			if( pPartial )
			{
				//qDebug() << "NEW PIN PARTIAL:" << TmpId << PinId;
			}

			pPinMap.insert( PinGlobalId, TmpId );

			PIN = context()->global()->createPin( PinName, PinLocalId, context()->findNode( uuid() ), pDirection, fugio::utils::string2uuid( PinCtl ), PinData );

			if( PIN )
			{
				if( PinPrivate *PP = qobject_cast<PinPrivate *>( PIN->qobject() ) )
				{
					PP->setGlobalId( TmpId );

					PP->setSettings( PinData );
				}

				PIN->loadSettings2( CFG, pPartial );

				PIN->setOrder( i );

				addPin( PIN );
			}
		}

		CFG.endGroup();
	}

	pSettings.endArray();
}

void NodePrivate::loadSettings2( QSettings &pSettings, QMap<QUuid, QUuid> &pPinMap, bool pPartial )
{
	Q_UNUSED( pPartial )

	setName( pSettings.value( "name", mName ).toString() );

	setActive( pSettings.value( "active", mActive ).toBool() );

	//-------------------------------------------------------------------------

	if( !control().isNull() )
	{
		pSettings.beginGroup( "control" );

		control()->loadSettings( pSettings );

		pSettings.endGroup();
	}

	//-------------------------------------------------------------------------

	loadPins2( pSettings, "inputs", PIN_INPUT, pPinMap, pPartial );

	//-------------------------------------------------------------------------

	loadPins2( pSettings, "outputs", PIN_OUTPUT, pPinMap, pPartial );

	//-------------------------------------------------------------------------

	qSort( mPinInputs.begin(), mPinInputs.end(), pinOrderFunction );
	qSort( mPinOutputs.begin(), mPinOutputs.end(), pinOrderFunction );

	//-------------------------------------------------------------------------

//	pSettings.beginGroup( "settings" );

//	foreach( const QString &K, pSettings.childKeys() )
//	{
//		QVariant	V = pSettings.value( K );

//		mSettings.insert( K, V );
//	}

//	pSettings.endGroup();
}

void NodePrivate::saveSettings1( QSettings &pSettings, bool pPartial )
{
	Q_UNUSED( pPartial )

	pSettings.beginGroup( name() );

	//-------------------------------------------------------------------------

	pSettings.setValue( "version", 1 );

	pSettings.setValue( "name", mName );
	pSettings.setValue( "uuid", mUUID.toString() );

	if( !mActive )
	{
		pSettings.setValue( "active", mActive );
	}

	//-------------------------------------------------------------------------

	pSettings.beginGroup( "control" );

	control()->saveSettings( pSettings );

	pSettings.endGroup();

	//-------------------------------------------------------------------------

	pSettings.beginGroup( "pins" );

	for( QList< QSharedPointer<fugio::PinInterface> >::iterator it = mPinInputs.begin() ; it != mPinInputs.end() ; it++ )
	{
		(*it)->saveSettings1( pSettings );
	}

	for( QList< QSharedPointer<fugio::PinInterface> >::iterator it = mPinOutputs.begin() ; it != mPinOutputs.end() ; it++ )
	{
		(*it)->saveSettings1( pSettings );
	}

	pSettings.endGroup();

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

void NodePrivate::saveSettings2( QSettings &pSettings, bool pPartial )
{
	Q_UNUSED( pPartial )

	pSettings.setValue( "version", 2 );

	pSettings.setValue( "name", mName );

	if( !mActive )
	{
		pSettings.setValue( "active", mActive );
	}

	//-------------------------------------------------------------------------
	// save input pins - not using Pin->order as there are duplicates!

	pSettings.beginWriteArray( "inputs" );

	int	inputId = 0;

	for( auto &P : mPinInputs )
	{
		pSettings.setArrayIndex( inputId++ );

		if( P->control() )
		{
			pSettings.setValue( fugio::utils::uuid2string( P->globalId() ), fugio::utils::uuid2string( P->controlUuid() ) );
		}
		else
		{
			pSettings.setValue( fugio::utils::uuid2string( P->globalId() ), fugio::utils::uuid2string( QUuid() ) );
		}
	}

	pSettings.endArray();

	//-------------------------------------------------------------------------
	// save output pins - not using Pin->order as there are duplicates!

	pSettings.beginWriteArray( "outputs" );

	int	outputId = 0;

	foreach( auto &P, mPinOutputs )
	{
		pSettings.setArrayIndex( outputId++ );

		if( P->control() )
		{
			pSettings.setValue( fugio::utils::uuid2string( P->globalId() ), fugio::utils::uuid2string( P->controlUuid() ) );
		}
		else
		{
			pSettings.setValue( fugio::utils::uuid2string( P->globalId() ), fugio::utils::uuid2string( QUuid() ) );
		}
	}

	pSettings.endArray();

	//-------------------------------------------------------------------------

	if( hasControl() )
	{
		pSettings.beginGroup( "control" );

		control()->saveSettings( pSettings );

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

void NodePrivate::updateOutputs( void )
{
	for( auto it = mPinMap.begin() ; it != mPinMap.end() ; it++ )
	{
		if( it.value()->direction() != PIN_OUTPUT )
		{
			continue;
		}

		mContext->updateNode( it.value()->connectedNode() );
	}
}

void NodePrivate::setSetting( const QString &pKey, const QVariant &pValue )
{
	mSettings.insert( pKey, pValue );
}

QVariant NodePrivate::setting( const QString &pKey, const QVariant &pDefault )
{
	return( mSettings.value( pKey, pDefault ) );
}

void NodePrivate::update(void )
{
	context()->updateNode( context()->findNode( uuid() ) );
}

void NodePrivate::setActive( bool pActive )
{
	if( mActive == pActive )
	{
		return;
	}

	mActive = pActive;

	emit activationChanged( context()->findNode( uuid() ) );
}

QList<fugio::NodeInterface::UuidPair> NodePrivate::pairedPins()
{
	QList<fugio::NodeInterface::UuidPair>	PinLst;

	for( QSharedPointer<fugio::PinInterface> PI : enumInputPins() )
	{
		if( PI->pairedUuid().isNull() )
		{
			continue;
		}

		PinLst << fugio::NodeInterface::UuidPair( PI->localId(), PI->pairedUuid() );
	}

	return( PinLst );
}

void NodePrivate::pairPins( QSharedPointer<fugio::PinInterface> pPin1, QSharedPointer<fugio::PinInterface> pPin2 )
{
	if( pPin1->pairedUuid() == pPin2->localId() && pPin2->pairedUuid() == pPin1->localId() )
	{
		return;
	}

	if( !pPin1->pairedUuid().isNull() && pPin1->pairedUuid() != pPin2->localId() )
	{
		unpairPins( pPin1->localId(), pPin1->pairedUuid() );
	}

	if( !pPin2->pairedUuid().isNull() && pPin2->pairedUuid() != pPin1->localId() )
	{
		unpairPins( pPin2->pairedUuid(), pPin1->localId() );
	}

	PinPrivate	*PP1 = qobject_cast<PinPrivate *>( pPin1->qobject() );
	PinPrivate	*PP2 = qobject_cast<PinPrivate *>( pPin2->qobject() );

	PP1->setPairedUuid( pPin2->localId() );
	PP2->setPairedUuid( pPin1->localId() );

	emit pinsPaired( pPin1, pPin2 );
}

void NodePrivate::pairPins( const QUuid &pLocalId1, const QUuid &pLocalId2 )
{
	QSharedPointer<fugio::PinInterface>	P1 = findPinByLocalId( pLocalId1 );
	QSharedPointer<fugio::PinInterface>	P2 = findPinByLocalId( pLocalId2 );

	if( P1 && P2 )
	{
		pairPins( P1, P2 );
	}
}

void NodePrivate::unpairPins( QSharedPointer<fugio::PinInterface> pPin1, QSharedPointer<fugio::PinInterface> pPin2 )
{
	PinPrivate	*PP1 = qobject_cast<PinPrivate *>( pPin1->qobject() );
	PinPrivate	*PP2 = qobject_cast<PinPrivate *>( pPin2->qobject() );

	PP1->setPairedUuid( QUuid() );
	PP2->setPairedUuid( QUuid() );

	emit pinsUnpaired( pPin1, pPin2 );
}

void NodePrivate::unpairPins(const QUuid &pLocalId1, const QUuid &pLocalId2)
{
	QSharedPointer<fugio::PinInterface>	P1 = findPinByLocalId( pLocalId1 );
	QSharedPointer<fugio::PinInterface>	P2 = findPinByLocalId( pLocalId2 );

	if( P1 && P2 )
	{
		unpairPins( P1, P2 );
	}
}

void NodePrivate::pinRenamed( const QUuid &pOld, const QUuid &pNew )
{
	QSharedPointer<fugio::PinInterface>	P = mPinMap.value( pOld );

	mPinMap.remove( pOld );

	mPinMap.insert( pNew, P );
}

bool NodePrivate::pinOrderFunction( const QSharedPointer<fugio::PinInterface> &p1, const QSharedPointer<fugio::PinInterface> &p2 )
{
	return( p1->order() < p2->order() );
}
