#ifndef JOINPOINTNODE_H
#define JOINPOINTNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>
#include <fugio/core/variant_interface.h>

class JoinPointNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Makes a Point value from raw values." )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Join_(Point)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE JoinPointNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~JoinPointNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputX;
	QSharedPointer<fugio::PinInterface>			 mPinInputY;

	QSharedPointer<fugio::PinInterface>			 mPinOutputPoint;
	fugio::VariantInterface						*mValOutputPoint;
};

#endif // JOINPOINTNODE_H
