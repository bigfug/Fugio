#ifndef FLIPFLOPNODE_H
#define FLIPFLOPNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

#include <fugio/core/variant_interface.h>

class FlipFlopNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Flip_Flop_(Logic)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE FlipFlopNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~FlipFlopNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputTrigger;
	QSharedPointer<fugio::PinInterface>			 mPinInputValue;

	QSharedPointer<fugio::PinInterface>			 mPinOutputValue;
	fugio::VariantInterface						*mValOutputValue;

	bool										 mValLst;
};

#endif // FLIPFLOPNODE_H
