#ifndef BITSTOBOOLNODE_H
#define BITSTOBOOLNODE_H

#include <QObject>
#include <QList>
#include <QBitArray>

#include <fugio/nodecontrolbase.h>

class BitsToBoolNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", tr( "Takes a BitArray and outputs a Bool when triggered" ) )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Bits_To_Bool" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE BitsToBoolNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~BitsToBoolNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputTrigger;
	QSharedPointer<fugio::PinInterface>			 mPinInputBits;

	QSharedPointer<fugio::PinInterface>			 mPinOutputBool;
	fugio::VariantInterface						*mValOutputBool;

	QSharedPointer<fugio::PinInterface>			 mPinOutputEmpty;

	QList<QBitArray>							 mBitDat;
	int											 mBitPos;
};

#endif // BITSTOBOOLNODE_H
