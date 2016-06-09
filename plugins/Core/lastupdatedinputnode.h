#ifndef LASTUPDATEDINPUTNODE_H
#define LASTUPDATEDINPUTNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

class LastUpdatedInputNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Number_Spread" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE LastUpdatedInputNode( QSharedPointer<fugio::NodeInterface> pNode);

	virtual ~LastUpdatedInputNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInput;
	QSharedPointer<fugio::PinInterface>			 mPinOutput;

	// NodeControlInterface interface
public:
	virtual QList<QUuid> pinAddTypesInput() const Q_DECL_OVERRIDE;
	virtual bool canAcceptPin(fugio::PinInterface *pPin) const Q_DECL_OVERRIDE;
};

#endif // LASTUPDATEDINPUTNODE_H
