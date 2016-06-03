#ifndef LISTSIZENODE_H
#define LISTSIZENODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

class ListSizeNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Gets the count of items in a list" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/ListSize" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE ListSizeNode( QSharedPointer<fugio::NodeInterface> pNode);

	virtual ~ListSizeNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputList;

	QSharedPointer<fugio::PinInterface>			 mPinOutputSize;

	fugio::VariantInterface						*mOutputSize;
};

#endif // LISTSIZENODE_H
