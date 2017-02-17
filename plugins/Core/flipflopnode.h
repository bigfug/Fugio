#ifndef FLIPFLOPNODE_H
#define FLIPFLOPNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

class FlipFlopNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Takes one input, routes it between two outputs based on boolean or trigger" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Flip_Flop" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE FlipFlopNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~FlipFlopNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputSwitch;
	QSharedPointer<fugio::PinInterface>			 mPinInputValue;

	QSharedPointer<fugio::PinInterface>			 mPinOutput1;
	QSharedPointer<fugio::PinInterface>			 mPinOutput2;

	bool										 mState;
};


#endif // FLIPFLOPNODE_H
