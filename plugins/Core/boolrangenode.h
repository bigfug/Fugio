#ifndef BOOLRANGENODE_H
#define BOOLRANGENODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

class BoolRangeNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Takes an input number between 0.0 and 1.0 and triggers one of its bool outputs." )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Bool_Range" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE BoolRangeNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~BoolRangeNode( void ) {}

	// NodeControlInterface interface

	virtual bool initialise( void ) Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual QList<QUuid> pinAddTypesOutput( void ) const Q_DECL_OVERRIDE;

	virtual bool canAcceptPin( fugio::PinInterface *pPin ) const Q_DECL_OVERRIDE;

	virtual QUuid pinAddControlUuid(fugio::PinInterface *pPin) const Q_DECL_OVERRIDE;

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputIndex;
	QSharedPointer<fugio::PinInterface>			 mPinInputTrigger;

	int											 mSelectedBool;
};

#endif // BOOLRANGENODE_H
