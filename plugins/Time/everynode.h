#ifndef EVERYNODE_H
#define EVERYNODE_H

#include <QObject>
#include <QTimer>

#include <fugio/core/variant_interface.h>

#include <fugio/nodecontrolbase.h>

class EveryNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Triggers its output pin every X number of milliseconds. X = input number" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Every" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit EveryNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~EveryNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface interface

public:
	virtual bool initialise( void ) Q_DECL_OVERRIDE;
	virtual bool deinitialise( void ) Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

private slots:
	void timeout( void );

private:
	QSharedPointer<fugio::PinInterface>	 mPinNumber;

	QSharedPointer<fugio::PinInterface>	 mPinTrigger;

	QTimer								*mTimer;
};

#endif // EVERYNODE_H
