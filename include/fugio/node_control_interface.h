#ifndef INTERFACE_NODE_CONTROL_H
#define INTERFACE_NODE_CONTROL_H

#include <QObject>
#include <QDebug>

#include <QUuid>

#include <fugio/global.h>

class QWidget;
class QSettings;
class QGraphicsItem;

FUGIO_NAMESPACE_BEGIN
class NodeInterface;
class PinInterface;

class NodeControlInterface
{
public:
	virtual ~NodeControlInterface( void ) { }

	virtual QObject *qobject( void ) = 0;

	virtual QGraphicsItem *guiItem( void ) = 0;

	virtual QWidget *gui( void ) = 0;

	virtual QSharedPointer<fugio::NodeInterface> node( void ) = 0;
	virtual QSharedPointer<fugio::NodeInterface> node( void ) const = 0;

	virtual void inputsUpdated( qint64 pTimeStamp ) = 0;

	virtual bool initialise( void ) = 0;

	virtual bool deinitialise( void ) = 0;

	virtual void loadSettings( QSettings &pSettings ) = 0;

	virtual void saveSettings( QSettings &pSettings ) const = 0;

	typedef struct AvailablePinEntry
	{
		QString		mName;
		QUuid		mType;
		QUuid		mUuid;

		AvailablePinEntry( const QString &pName, const QUuid &pType = QUuid(), const QUuid &pUuid = QUuid() )
			: mName( pName ), mType( pType ), mUuid( pUuid )
		{
		}

		bool operator < ( const AvailablePinEntry &pAPE ) const
		{
			return ( mName < pAPE.mName );
		}

		bool operator == ( const QString &pString ) const
		{
			return( mName == pString );
		}

		bool operator == ( const AvailablePinEntry &pAPE ) const
		{
			return( mName == pAPE.mName );
		}
	} AvailablePinEntry;

	virtual QStringList availableInputPins( void ) const = 0;
	virtual QList<AvailablePinEntry> availableOutputPins( void ) const = 0;

	virtual QList<QUuid> pinAddTypesInput( void ) const = 0;
	virtual QList<QUuid> pinAddTypesOutput( void ) const = 0;

	virtual bool canAcceptPin( fugio::PinInterface *pPin ) const = 0;

	virtual QUuid pinAddControlUuid( fugio::PinInterface *pPin ) const = 0;

	virtual bool mustChooseNamedInputPin( void ) const = 0;
	virtual bool mustChooseNamedOutputPin( void ) const = 0;

	virtual bool pinShouldAutoRename( fugio::PinInterface *pPin ) const = 0;

	virtual bool wasInitialiseCalled( void ) const = 0;
	virtual bool wasDeinitialiseCalled( void ) const = 0;

	enum Option {
        NoOptions	= 0,
        CanShow		= 1 << 0,
    };

	Q_DECLARE_FLAGS( Options, Option )

	virtual bool hasOption( Option pOption ) const = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::NodeControlInterface, "com.bigfug.fugio.NodeControl/1.0" )

#endif // INTERFACE_NODE_CONTROL_H
