#ifndef LINEBUFFERNODE_H
#define LINEBUFFERNODE_H

#include <QObject>
#include <QRegExp>

#include <fugio/node_interface.h>
#include <fugio/pin_interface.h>

#include <fugio/core/variant_interface.h>

#include <fugio/core/uuid.h>

#include <fugio/nodecontrolbase.h>

class LineBufferNode : public fugio::NodeControlBase
{
	Q_OBJECT

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Breaks up input into strings" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Line_Buffer" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit LineBufferNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~LineBufferNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp );

private:
	QSharedPointer<fugio::PinInterface>			 mPinInput;
	QSharedPointer<fugio::PinInterface>			 mPinInputSplit;
	QSharedPointer<fugio::PinInterface>			 mPinInputReset;

	QSharedPointer<fugio::PinInterface>			 mPinOutput;
	fugio::VariantInterface						*mValOutput;

	QString										 mBuffer;
};

#endif // LINEBUFFERNODE_H
