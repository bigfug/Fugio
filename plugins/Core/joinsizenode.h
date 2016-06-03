#ifndef JOINSIZENODE_H
#define JOINSIZENODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>
#include <fugio/core/variant_interface.h>

class JoinSizeNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Makes a Size node from raw values." )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Join_Size" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE JoinSizeNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~JoinSizeNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinWidth;
	QSharedPointer<fugio::PinInterface>			 mPinHeight;

	QSharedPointer<fugio::PinInterface>			 mPinSize;

	fugio::VariantInterface						*mSize;
};

#endif // JOINSIZENODE_H
