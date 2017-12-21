#ifndef INTTOBITSNODE_H
#define INTTOBITSNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

#include <fugio/core/variant_interface.h>

class IntToBitsNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Converts a integer to a bit array" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "IntToBits" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE IntToBitsNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~IntToBitsNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputInteger;
	QSharedPointer<fugio::PinInterface>			 mPinInputBits;

	QSharedPointer<fugio::PinInterface>			 mPinOutputBitArray;
	fugio::VariantInterface						*mValOutputBitArray;
};

#endif // INTTOBITSNODE_H
