#ifndef FUGIO_PIN_H
#define FUGIO_PIN_H

#include <QObject>
#include <QSettings>
#include <QUuid>

#include "global.h"

FUGIO_NAMESPACE_BEGIN

class PinInterface;

class PinSignals : public QObject
{
	Q_OBJECT

signals:
	void renamed( const QUuid &pOld, const QUuid &pNew );

	void nameChanged( QSharedPointer<fugio::PinInterface> pPin );

	void nameChanged( const QString &pName );

	void updated( void );

	void linked( QSharedPointer<fugio::PinInterface> pPin );

	void unlinked( QSharedPointer<fugio::PinInterface> pPin );

	void valueChanged( const QVariant &pValue );

	void displayLabelChanged( const QString &pDisplayLabel );

protected:
	PinSignals( QObject *pParent = 0 );

	virtual ~PinSignals( void );
};

FUGIO_NAMESPACE_END

#endif // FUGIO_PIN_H
