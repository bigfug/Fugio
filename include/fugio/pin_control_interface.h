#ifndef INTERFACE_PIN_CONTROL_H
#define INTERFACE_PIN_CONTROL_H

#include <QtPlugin>
#include <QDebug>
#include <QUuid>
#include <QSharedPointer>

#include "global.h"

class QSettings;
class QWidget;
class QTextStream;

FUGIO_NAMESPACE_BEGIN

class PinInterface;

class PinControlInterface
{
public:
	virtual ~PinControlInterface( void )
	{
		//qDebug() << "~fugio::PinControlInterface";
	}

	virtual QObject *qobject( void ) = 0;

	virtual const QObject *qobject( void ) const = 0;

	virtual QWidget *gui( void ) = 0;

	virtual void loadSettings( QSettings &pSettings ) = 0;

	virtual void saveSettings( QSettings &pSettings ) const = 0;

	virtual QString toString( void ) const = 0;

	virtual QString description( void ) const = 0;

	virtual void dataExport( QTextStream &pDataStream ) const = 0;

	virtual void dataImport( QTextStream &pDataStream ) = 0;

	virtual QSharedPointer<fugio::PinInterface> pin( void ) const = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::PinControlInterface, "com.bigfug.fugio.pincontrol/1.0" )

#endif // INTERFACE_PIN_CONTROL_H
