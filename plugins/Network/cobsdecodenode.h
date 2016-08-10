#ifndef COBSDECODENODE_H
#define COBSDECODENODE_H

#include <fugio/nodecontrolbase.h>

class COBSDecodeNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Decodes arrays of data into a decodable single stream" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/COBS_Decode" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE COBSDecodeNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~COBSDecodeNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInput;

	QSharedPointer<fugio::PinInterface>			 mPinInputReset;

	QSharedPointer<fugio::PinInterface>			 mPinOutput;
	fugio::VariantInterface						*mValOutput;

	QByteArray									 mCurPkt;
	quint8										 mCurLen;
	quint8										 mCurPos;
};

#endif // COBSDECODENODE_H
