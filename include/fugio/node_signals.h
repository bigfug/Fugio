#ifndef FUGIO_NODE_H
#define FUGIO_NODE_H

#include <QObject>

#include "global.h"
#include "node_interface.h"
#include "pin_interface.h"

//class fugio::NodeInterface;
//class fugio::PinInterface;

FUGIO_NAMESPACE_BEGIN

class NodeSignals : public QObject
{
	Q_OBJECT

signals:
	void pinAdded( QSharedPointer<fugio::PinInterface> pPin );
	void pinRemoved( QSharedPointer<fugio::PinInterface> pPin );

	void pinAdded( QSharedPointer<fugio::NodeInterface> pNode, QSharedPointer<fugio::PinInterface> pPin );
	void pinRemoved( QSharedPointer<fugio::NodeInterface> pNode, QSharedPointer<fugio::PinInterface> pPin );
	void controlChanged( QSharedPointer<fugio::NodeInterface> pNode );
	void nameChanged( QSharedPointer<fugio::NodeInterface> pNode );
	void nameChanged( const QString &pName );
	void activationChanged( QSharedPointer<fugio::NodeInterface> pNode );
	void statusUpdated( QSharedPointer<fugio::NodeInterface> pNode );
	void statusUpdated( void );

	void pinLinked( QSharedPointer<fugio::PinInterface> pPinSrc, QSharedPointer<fugio::PinInterface> pPinDst );
	void pinUnlinked( QSharedPointer<fugio::PinInterface> pPinSrc, QSharedPointer<fugio::PinInterface> pPinDst );

	void pinsPaired( QSharedPointer<fugio::PinInterface> pPin1, QSharedPointer<fugio::PinInterface> pPin2 );
	void pinsUnpaired( QSharedPointer<fugio::PinInterface> pPin1, QSharedPointer<fugio::PinInterface> pPin2 );

protected:
	NodeSignals( QObject *pParent = 0 );

	virtual ~NodeSignals( void );
};

FUGIO_NAMESPACE_END

#endif // FUGIO_NODE_H
