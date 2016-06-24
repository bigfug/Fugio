#ifndef ANYTRIGGERNODE_H
#define ANYTRIGGERNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

class AnyTriggerNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Takes any number of trigger inputs and fires off a trigger if any of them activate." )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Any_Trigger" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE AnyTriggerNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~AnyTriggerNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual QList<QUuid> pinAddTypesInput() const Q_DECL_OVERRIDE;

	virtual bool canAcceptPin( fugio::PinInterface *pPin ) const Q_DECL_OVERRIDE;

	virtual bool pinShouldAutoRename( fugio::PinInterface *pPin ) const Q_DECL_OVERRIDE;

protected:
	QSharedPointer<fugio::PinInterface>			 mPinOutput;
};

#endif // ANYTRIGGERNODE_H
