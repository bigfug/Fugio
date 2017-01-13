#ifndef COUNTERNODE_H
#define COUNTERNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

class CounterNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Counter" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Counter" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE CounterNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~CounterNode( void ) {}

	// NodeControlInterface interface

public:
	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputTrigger;
	QSharedPointer<fugio::PinInterface>			 mPinInputReset;

	QSharedPointer<fugio::PinInterface>			 mPinOutputCount;

	fugio::VariantInterface						*mOutputCount;
};

#endif // COUNTERNODE_H
