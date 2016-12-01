#ifndef TRIGGERARRAYNODE_H
#define TRIGGERARRAYNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

class TriggerArrayNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Each input trigger activates the next trigger in the output array" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Trigger_Array" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE TriggerArrayNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~TriggerArrayNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual QList<QUuid> pinAddTypesOutput() const Q_DECL_OVERRIDE;

	virtual bool canAcceptPin( fugio::PinInterface *pPin ) const Q_DECL_OVERRIDE;

	virtual bool pinShouldAutoRename( fugio::PinInterface *pPin ) const Q_DECL_OVERRIDE;

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputTrigger;
	QSharedPointer<fugio::PinInterface>			 mPinInputReset;

	int											 mIndex;
};

#endif // TRIGGERARRAYNODE_H
