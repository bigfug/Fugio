#ifndef MULTIPLEXORNODE_H
#define MULTIPLEXORNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

#include <fugio/core/variant_interface.h>

class MultiplexorNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Logic Multiplexor" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Multiplexor" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE MultiplexorNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~MultiplexorNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputA;
	QSharedPointer<fugio::PinInterface>			 mPinInputB;
	QSharedPointer<fugio::PinInterface>			 mPinInputSel;

	QSharedPointer<fugio::PinInterface>			 mPinOutputValue;
	fugio::VariantInterface						*mValOutputValue;
};

#endif // MULTIPLEXORNODE_H
