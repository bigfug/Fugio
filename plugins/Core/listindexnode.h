#ifndef LISTINDEXNODE_H
#define LISTINDEXNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

class ListIndexNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Extracts a single item from a list" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/List_Index" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE ListIndexNode( QSharedPointer<fugio::NodeInterface> pNode);

	virtual ~ListIndexNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputList;
	QSharedPointer<fugio::PinInterface>			 mPinInputIndex;

	QSharedPointer<fugio::PinInterface>			 mPinOutputValue;
};


#endif // LISTINDEXNODE_H
