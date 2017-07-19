#ifndef BOOLTOBITSNODE_H
#define BOOLTOBITSNODE_H

#include <QObject>
#include <QList>
#include <QBitArray>

#include <fugio/nodecontrolbase.h>

class BoolToBits : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", tr( "Reads the input bool when triggered and outputs a bit array" ) )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Bool_To_Bits" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE BoolToBits( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~BoolToBits( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputTrigger;
	QSharedPointer<fugio::PinInterface>			 mPinInputBool;
	QSharedPointer<fugio::PinInterface>			 mPinInputCount;

	QSharedPointer<fugio::PinInterface>			 mPinOutputBits;
	fugio::VariantInterface						*mValOutputBits;

	QBitArray									 mBitDat;
	int											 mBitPos;
};


#endif // BOOLTOBITSNODE_H
