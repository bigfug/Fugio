#ifndef DELAYNODE_H
#define DELAYNODE_H

#include <QObject>

#include <fugio/core/variant_interface.h>

#include <fugio/nodecontrolbase.h>

class DelayNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Takes an input trigger, waits for the specified amount of milliseconds, then triggers its output" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Beat_Tap" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit DelayNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~DelayNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp );

private slots:
	void frameStart( qint64 pTimeStamp );

private:
	QSharedPointer<fugio::PinInterface>	 mPinNumber;

	QSharedPointer<fugio::PinInterface>	 mPinTrigger;

	qint64							 mTriggerTime;
};

#endif // DELAYNODE_H
