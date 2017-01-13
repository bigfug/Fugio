#ifndef CROSSPRODUCENODE_H
#define CROSSPRODUCENODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

#include <fugio/core/variant_interface.h>

class CrossProductNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Calculates the cross product from two Vector3" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Cross Product" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE CrossProductNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~CrossProductNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInput1;
	QSharedPointer<fugio::PinInterface>			 mPinInput2;

	QSharedPointer<fugio::PinInterface>			 mPinOutput;
	fugio::VariantInterface						*mValOutput;
};

#endif // CROSSPRODUCENODE_H
