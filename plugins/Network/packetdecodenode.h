#ifndef PACKETDECODENODE_H
#define PACKETDECODENODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

class PacketDecodeNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", tr( "Decodes packets into data" ) )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Packet_Decode" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE PacketDecodeNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~PacketDecodeNode( void ) {}

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
	QSharedPointer<fugio::PinInterface>			 mPinInputPackets;

	QSharedPointer<fugio::PinInterface>			 mPinOutputData;
	fugio::VariantInterface						*mValOutputData;
};


#endif // PACKETDECODENODE_H
