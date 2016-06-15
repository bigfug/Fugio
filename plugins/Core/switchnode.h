#ifndef SWITCHNODE_H
#define SWITCHNODE_H

#include <QObject>

#include "fugio/node_interface.h"
#include "fugio/node_control_interface.h"
#include "fugio/pin_interface.h"
#include "fugio/pin_control_interface.h"

#include <fugio/core/variant_interface.h>

#include <fugio/core/uuid.h>

#include <fugio/nodecontrolbase.h>

class SwitchNode : public fugio::NodeControlBase
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit SwitchNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~SwitchNode( void );

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual bool canAcceptPin( fugio::PinInterface *pPin ) const Q_DECL_OVERRIDE;

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputValue;
	QSharedPointer<fugio::PinInterface>			 mPinOutputValue;
	int											 mCurrentIndex;
};

#endif // SWITCHNODE_H
