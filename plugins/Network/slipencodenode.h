#ifndef SLIPENCODENODE_H
#define SLIPENCODENODE_H

#include <fugio/nodecontrolbase.h>

class SLIPEncodeNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Encodes arrays of data into a decodable single stream" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/SLIP_Decode" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE SLIPEncodeNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~SLIPEncodeNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

private:
	void processByteArray( const QByteArray &pDatInp, QByteArray &pDatOut );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInput;
	QSharedPointer<fugio::PinInterface>			 mPinOutput;
	fugio::VariantInterface						*mValOutput;
};

#endif // SLIPENCODENODE_H
