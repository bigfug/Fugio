#ifndef PROCESSCLICKNODE_H
#define PROCESSCLICKNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

#include <fugio/core/variant_interface.h>

class ProcessClickNode : public fugio::NodeControlBase
{
	Q_OBJECT

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Process_Click" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE ProcessClickNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~ProcessClickNode( void ) {}

	// NodeControlInterface interface
public:
	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputBoolean;

	QSharedPointer<fugio::PinInterface>			 mPinOutputDown;
	QSharedPointer<fugio::PinInterface>			 mPinOutputUp;
	QSharedPointer<fugio::PinInterface>			 mPinOutputDoubleClick;

	qint64										 mLastClick;
	bool										 mLastValue;
};

#endif // PROCESSCLICKNODE_H
