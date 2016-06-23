#ifndef SLIPDECODENODE_H
#define SLIPDECODENODE_H

#include <fugio/nodecontrolbase.h>

class SLIPDecodeNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Encodes arrays of data into a decodable single stream" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/SLIP_Decode" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE SLIPDecodeNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~SLIPDecodeNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInput;
	QSharedPointer<fugio::PinInterface>			 mPinOutput;
	fugio::VariantInterface						*mValOutput;

	QByteArray									 mCurPkt;
	bool										 mEscChr;
};

#endif // SLIPDECODENODE_H
