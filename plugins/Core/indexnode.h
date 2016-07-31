#ifndef INDEXNODE_H
#define INDEXNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

class IndexNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Takes any number of input pins and outputs the index and value of the most recently active." )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Index" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE IndexNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~IndexNode( void ) {}

	// NodeControlInterface interface
public:
	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual QList<QUuid> pinAddTypesInput() const Q_DECL_OVERRIDE;

	virtual bool canAcceptPin( fugio::PinInterface *pPin ) const Q_DECL_OVERRIDE;

	virtual bool pinShouldAutoRename( fugio::PinInterface *pPin ) const Q_DECL_OVERRIDE;

protected:
	QSharedPointer<fugio::PinInterface>			 mPinOutputIndex;
	QSharedPointer<fugio::PinInterface>			 mPinOutputValue;

	fugio::VariantInterface						*mOutputIndex;

	qint64										 mLastUpdate;
};

#endif // INDEXNODE_H
