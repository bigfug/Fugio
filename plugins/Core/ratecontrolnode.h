#ifndef RATECONTROLNODE_H
#define RATECONTROLNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

class RateControlNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Limits how fast a pin updates." )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Rate_Control" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE RateControlNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~RateControlNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated(qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInput;
	QSharedPointer<fugio::PinInterface>			 mPinTrigger;

	QSharedPointer<fugio::PinInterface>			 mPinOutput;

	bool										 mHaveInput;
	bool										 mHaveTrigger;

	qint64										 mTimeInput;
	qint64										 mTimeTrigger;
};

#endif // RATECONTROLNODE_H
