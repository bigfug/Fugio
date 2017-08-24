#ifndef SPLITLISTNODE_H
#define SPLITLISTNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

class SplitListNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Split_(List)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE SplitListNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~SplitListNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp);

//	virtual QList<QUuid> pinAddTypesInput() const;

//	virtual bool canAcceptPin( fugio::PinInterface *pPin ) const;

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputList;
};


#endif // SPLITLISTNODE_H
