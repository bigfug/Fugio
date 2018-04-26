#ifndef INTERFACE_APP_H
#define INTERFACE_APP_H

#include <QObject>
#include <QUuid>
#include <QSharedPointer>
#include <QList>
#include <QElapsedTimer>

#include "pin_interface.h"

#include "global.h"

class QWidget;
class QDir;

FUGIO_NAMESPACE_BEGIN
class GlobalSignals;
class ContextInterface;
class NodeInterface;
class NodeControlInterface;
class DeviceFactoryInterface;
class EditInterface;
FUGIO_NAMESPACE_END

FUGIO_NAMESPACE_BEGIN

typedef struct ClassEntry
{
	enum Flag
	{
		None = 0x0,
		Deprecated = 1 << 16
	};

	Q_DECLARE_FLAGS( Flags, Flag )

	QString				 mName;
	QString				 mGroup;
	QUuid				 mUuid;
	const QMetaObject	*mMetaObject;
	ClassEntry::Flags	 mFlags;

	ClassEntry( void )
		: mMetaObject( 0 )
	{

	}

	ClassEntry( const QString &pName, const QUuid &pUuid, const QMetaObject *pMetaObject )
		: mName( pName ), mUuid( pUuid ), mMetaObject( pMetaObject ), mFlags( ClassEntry::None )
	{

	}

	ClassEntry( const QString &pName, const QString &pGroup, const QUuid &pUuid, const QMetaObject *pMetaObject )
		: mName( pName ), mGroup( pGroup ), mUuid( pUuid ), mMetaObject( pMetaObject ), mFlags( ClassEntry::None )
	{

	}

	ClassEntry( const QString &pName, const QString &pGroup, ClassEntry::Flags pFlags, const QUuid &pUuid, const QMetaObject *pMetaObject )
		: mName( pName ), mGroup( pGroup ), mUuid( pUuid ), mMetaObject( pMetaObject ), mFlags( pFlags )
	{

	}

	QString friendlyName( void ) const
	{
		QString		LocalName = mName;

		if( !mGroup.isEmpty() )
		{
			LocalName = QString( "%1 (%2)" ).arg( LocalName ).arg( mGroup );
		}

		return( LocalName );
	}

} ClassEntry;

Q_DECLARE_OPERATORS_FOR_FLAGS( ClassEntry::Flags )

typedef QList<ClassEntry>	ClassEntryList;

class GlobalInterface
{
public:
	virtual ~GlobalInterface( void ) {}

	virtual fugio::GlobalSignals *qobject( void ) = 0;

	virtual void loadConfig( QSettings &pSettings ) = 0;
	virtual void saveConfig( QSettings &pSettings ) const = 0;

	virtual void clear( void ) = 0;

	virtual void loadPlugins( QDir pDir ) = 0;
	virtual void initialisePlugins( void ) = 0;
	virtual void unloadPlugins( void ) = 0;

	virtual qint64 timestamp( void ) const = 0;				// arbitrary global timestamp that always increases (only valid on local machine)

	//-------------------------------------------------------------------------
	// Patch Execution

	virtual void start( void ) = 0;			// start global execution timer
	virtual void stop( void ) = 0;			// stop global execution timer

	virtual QThread *thread( void ) = 0;

	virtual void executeFrame( void ) = 0;	// only call for single step execution

	virtual void scheduleFrame( void ) = 0; // schedule a frame execution

	//-------------------------------------------------------------------------
	// Command line values

	virtual bool commandLineDefined( const QString &pKey ) const = 0;
	virtual QString commandLineValue( const QString &pKey ) const = 0;

	virtual void setCommandLineValues( const QMap<QString,QString> &pValueMap ) = 0;

	//-------------------------------------------------------------------------
	// Paths

	virtual QString sharedDataPath( void ) const = 0;

	//-------------------------------------------------------------------------

	virtual QStringList loadedPluginNames( void ) const = 0;

	virtual void setEnabledPlugins( QStringList pEnabledPlugins ) = 0;
	virtual void setDisabledPlugins( QStringList pEnabledPlugins ) = 0;

	virtual void enablePlugin( QString pPluginName ) = 0;
	virtual void disablePlugin( QString pPluginName ) = 0;

	//-------------------------------------------------------------------------
	// Universe

	virtual void setUniversalTimeServer( const QString &pString, int pPort ) = 0;

	virtual qint64 universalTimestamp( void ) const = 0;	// can't be compared with timestamp(), can jump forward and back

	// convert between global and universal timestamps

	virtual qint64 universalToGlobal( qint64 pTimeStamp ) const = 0;
	virtual qint64 globalToUniversal( qint64 pTimeStamp ) const = 0;

	virtual void sendToUniverse( qint64 pTimeStamp, const QUuid &pUuid, const QString &pName, const QUuid &pType, const QByteArray &pByteArray ) = 0;

	virtual qint64 universeData( qint64 pTimeStamp, const QUuid &pUuid, QString &pName, QUuid &pType, QByteArray &pByteArray ) const = 0;

	typedef struct UniverseEntry
	{
		QString				mName;
		QUuid				mUuid;
		QUuid				mType;
	} UniverseEntry;

	virtual QList<UniverseEntry> universeEntries( void ) const = 0;

	//-------------------------------------------------------------------------
	// Pause global execution

	virtual bool isPaused( void ) const = 0;

	virtual void pause( void ) = 0;

	virtual void unpause( void ) = 0;

	//-------------------------------------------------------------------------
	// Interfaces

	virtual void registerInterface( const QUuid &pUuid, QObject *pInterface ) = 0;
	virtual void unregisterInterface( const QUuid &pUuid ) = 0;

	virtual QObject *findInterface( const QUuid &pUuid ) = 0;

	//-------------------------------------------------------------------------
	// Plugin register node/pin classes

	virtual void registerNodeClasses( const fugio::ClassEntryList &pNodes ) = 0;
	virtual void unregisterNodeClasses( const fugio::ClassEntryList &pNodes ) = 0;

	virtual void registerNodeClasses( const fugio::ClassEntry pNodes[] ) = 0;
	virtual void unregisterNodeClasses( const fugio::ClassEntry pNodes[] ) = 0;

	virtual void registerPinClasses( const fugio::ClassEntryList &pNodes ) = 0;
	virtual void unregisterPinClasses( const fugio::ClassEntryList &pNodes ) = 0;

	virtual void registerPinClasses( const fugio::ClassEntry pNodes[] ) = 0;
	virtual void unregisterPinClasses( const fugio::ClassEntry pNodes[] ) = 0;

	//-------------------------------------------------------------------------
	// Nodes and Pins

	virtual const QMetaObject *findNodeMetaObject( const QString &pClassName ) const = 0;

	virtual const QMetaObject *findNodeMetaObject( const QUuid &pNodeUuid ) const = 0;
	virtual const QMetaObject *findPinMetaObject( const QUuid &pPinUuid ) const = 0;

	virtual fugio::ClassEntry findNodeClassEntry( const QUuid &pNodeUuid ) const = 0;

	virtual QString nodeName( const QUuid &pUuid ) const = 0;
	virtual QString pinName( const QUuid &pUuid ) const = 0;

	virtual QUuid findNodeByClass( const QString &pClassName ) const = 0;
	virtual QUuid findPinByClass( const QString &pClassName ) const = 0;

	virtual QStringList pinNames( void ) const = 0;

	virtual QMap<QUuid, QString> pinIds( void ) const = 0;

	//-------------------------------------------------------------------------
	// Contexts

	virtual QSharedPointer<fugio::ContextInterface> newContext( void ) = 0;

	virtual void delContext( QSharedPointer<fugio::ContextInterface> pContext ) = 0;

	virtual QList< QSharedPointer<fugio::ContextInterface> > contexts( void ) = 0;

	//-------------------------------------------------------------------------
	// Device Factory

	virtual void registerDeviceFactory( fugio::DeviceFactoryInterface *pFactory ) = 0;
	virtual void unregisterDeviceFactory( fugio::DeviceFactoryInterface *pFactory ) = 0;

	virtual QStringList deviceFactoryMenuTextList( void ) const = 0;
	virtual void deviceFactoryGui( QWidget *pParent, const QString &pMenuText ) = 0;

	//-------------------------------------------------------------------------
	// Joining and Splitting

	virtual void registerPinSplitter( const QUuid &pPinId, const QUuid &pNodeId ) = 0;
	virtual void registerPinJoiner( const QUuid &pPinId, const QUuid &pNodeId ) = 0;

	virtual QList<QUuid> pinSplitters( const QUuid &pPinId ) const = 0;
	virtual QList<QUuid> pinJoiners( const QUuid &pPinId ) const = 0;

	//-------------------------------------------------------------------------
	// QMetaType

	virtual QUuid findPinForMetaType( QMetaType::Type pType ) const = 0;
	virtual void registerPinForMetaType( const QUuid &pUuid, QMetaType::Type pType ) = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::GlobalInterface, "com.bigfug.fugio.global/1.1" )

#if !defined(FUGIOLIB_LIBRARY)
FUGIO_NAMESPACE_BEGIN
FUGIOLIBSHARED_IMPORT fugio::GlobalInterface *fugio( void );
FUGIO_NAMESPACE_END
#endif

#endif // INTERFACE_APP_H
