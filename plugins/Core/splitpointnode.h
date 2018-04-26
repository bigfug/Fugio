#ifndef SPLITPOINTNODE_H
#define SPLITPOINTNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>
#include <fugio/core/variant_interface.h>

class SplitPointNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Splits a point into its raw values." )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Split_(Point)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE SplitPointNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~SplitPointNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputPoint;

	QSharedPointer<fugio::PinInterface>			 mPinOutputX;
	QSharedPointer<fugio::PinInterface>			 mPinOutputY;

	fugio::VariantInterface						*mValOutputX;
	fugio::VariantInterface						*mValOutputY;
};

#endif // SPLITPOINTNODE_H
