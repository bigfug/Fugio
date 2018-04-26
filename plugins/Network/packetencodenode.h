#ifndef PACKETENCODENODE_H
#define PACKETENCODENODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

#include <fugio/core/variant_interface.h>

class PacketEncodeNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", tr( "Encodes data into packets" ) )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Packet_Encode" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE PacketEncodeNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~PacketEncodeNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

private:
	void processVariant( QVariant V );

	void processByteArray( QByteArray A );

	typedef struct PktHdr
	{
		quint16		mType;
		quint16		mLength;
		quint16		mNumber;
		quint16		mACK;
		quint32		mCRC;
	} PktHdr;

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputData;

	QSharedPointer<fugio::PinInterface>			 mPinOutputPacket;
	fugio::VariantInterface						*mValOutputPacket;
};

#endif // PACKETENCODENODE_H
