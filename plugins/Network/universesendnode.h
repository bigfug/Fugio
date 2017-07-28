#ifndef UNIVERSESENDNODE_H
#define UNIVERSESENDNODE_H

#include <fugio/nodecontrolbase.h>

class UniverseSendNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Sends pins to the universe" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Universe_Send" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE UniverseSendNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~UniverseSendNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual bool canAcceptPin(fugio::PinInterface *pPin) const Q_DECL_OVERRIDE;
};

#endif // UNIVERSESENDNODE_H
