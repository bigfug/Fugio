#ifndef SERIALDECODERNODE_H
#define SERIALDECODERNODE_H

#include <QObject>
#include <QBitArray>

#include <fugio/nodecontrolbase.h>

class SerialDecoderNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Serial Decoder" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Serial_Decoder" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE SerialDecoderNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~SerialDecoderNode( void ) {}

	// NodeControlInterface interface

public:
	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputBits;

	QSharedPointer<fugio::PinInterface>			 mPinOutputData;
	fugio::VariantInterface						*mValOutputData;

	QBitArray									 mBitBuf;
};

#endif // SERIALDECODERNODE_H
