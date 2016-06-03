#ifndef DOTPRODUCTNODE_H
#define DOTPRODUCTNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

#include <fugio/core/variant_interface.h>

class DotProductNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Calculates the dot product from two Vector3" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Dot_Product" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE DotProductNode( QSharedPointer<fugio::NodeInterface> pNode);

	virtual ~DotProductNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated(qint64 pTimeStamp);

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInput1;
	QSharedPointer<fugio::PinInterface>			 mPinInput2;

	QSharedPointer<fugio::PinInterface>			 mPinOutput;
	fugio::VariantInterface						*mValOutput;
};

#endif // DOTPRODUCTNODE_H
