#ifndef SERIALENCODERNODE_H
#define SERIALENCODERNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

class SerialEncoderNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Serial Encoder" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Serial_Encoder" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE SerialEncoderNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~SerialEncoderNode( void ) {}

	// NodeControlInterface interface

public:
	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputData;

	QSharedPointer<fugio::PinInterface>			 mPinOutputBits;
	fugio::VariantInterface						*mValOutputBits;
};

#endif // SERIALENCODERNODE_H
