#ifndef INTERFACE_NODE_CONTROL_H
#define INTERFACE_NODE_CONTROL_H

#include <QObject>
#include <QDebug>

#include <QUuid>

#include <fugio/global.h>

class QWidget;
class QSettings;

FUGIO_NAMESPACE_BEGIN
class NodeInterface;
class PinInterface;

class NodeControlInterface
{
public:
	virtual ~NodeControlInterface( void ) { }

	virtual QObject *qobject( void ) = 0;

	virtual QWidget *gui( void ) = 0;

	virtual QSharedPointer<fugio::NodeInterface> node( void ) = 0;

	virtual void inputsUpdated( qint64 pTimeStamp ) = 0;

	virtual bool initialise( void ) = 0;

	virtual bool deinitialise( void ) = 0;

	virtual void loadSettings( QSettings &pSettings ) = 0;

	virtual void saveSettings( QSettings &pSettings ) const = 0;

	typedef struct AvailablePinEntry
	{
		QString		mName;
		QUuid		mType;

		AvailablePinEntry( const QString &pName, const QUuid &pUuid = QUuid() )
			: mName( pName ), mType( pUuid )
		{
		}

		bool operator < (const AvailablePinEntry& str) const
		{
			return ( mName < str.mName );
		}

		bool operator == (const QString& str) const
		{
			return ( mName == str );
		}

		bool operator == (const AvailablePinEntry& str) const
		{
			return ( mName == str.mName );
		}
	} AvailablePinEntry;

	virtual QStringList availableInputPins( void ) const = 0;
	virtual QList<AvailablePinEntry> availableOutputPins( void ) const = 0;

	virtual QString helpUrl( void ) const = 0;

	virtual QList<QUuid> pinAddTypesInput( void ) const = 0;
	virtual QList<QUuid> pinAddTypesOutput( void ) const = 0;

	virtual bool canAcceptPin( fugio::PinInterface *pPin ) const = 0;

	virtual QUuid pinAddControlUuid( fugio::PinInterface *pPin ) const = 0;

	virtual bool mustChooseNamedInputPin( void ) const = 0;
	virtual bool mustChooseNamedOutputPin( void ) const = 0;

	virtual bool pinShouldAutoRename( fugio::PinInterface *pPin ) const = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::NodeControlInterface, "com.bigfug.fugio.NodeControl/1.0" )

#endif // INTERFACE_NODE_CONTROL_H
