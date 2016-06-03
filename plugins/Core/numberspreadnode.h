#ifndef NUMBERSPREADNODE_H
#define NUMBERSPREADNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

class NumberSpreadNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Number_Spread" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE NumberSpreadNode( QSharedPointer<fugio::NodeInterface> pNode);

	virtual ~NumberSpreadNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

	virtual QList<QUuid> pinAddTypesOutput() const;

	virtual bool canAcceptPin( fugio::PinInterface *pPin ) const;

protected:
	QSharedPointer<fugio::PinInterface>			 mPinValue;
	QSharedPointer<fugio::PinInterface>			 mPinCenter;
	QSharedPointer<fugio::PinInterface>			 mPinSpread;
};

#endif // NUMBERSPREADNODE_H
