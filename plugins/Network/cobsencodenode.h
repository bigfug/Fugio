#ifndef COBSENCODENODE_H
#define COBSENCODENODE_H

#include <fugio/nodecontrolbase.h>

class COBSEncodeNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Encodes arrays of data into a decodable single stream" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Cobs_Encode" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE COBSEncodeNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~COBSEncodeNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

private:
	void processByteArray( const QByteArray &pDatInp, QByteArray &pDatOut );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInput;
	QSharedPointer<fugio::PinInterface>			 mPinOutput;
	fugio::VariantInterface						*mValOutput;
};

#endif // COBSENCODENODE_H
