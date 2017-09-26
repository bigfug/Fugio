#ifndef NANDBITSNODE_H
#define NANDBITSNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

#include <fugio/core/variant_interface.h>

class NandBitsNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Nands all the input booleans together." )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Nand_(Logic)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE NandBitsNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~NandBitsNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputBits1;
	QSharedPointer<fugio::PinInterface>			 mPinInputBits2;

	QSharedPointer<fugio::PinInterface>			 mPinOutputBits;
	fugio::VariantInterface						*mValOutputBits;
};

#endif // NANDBITSNODE_H
