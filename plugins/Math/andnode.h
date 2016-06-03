#ifndef ANDNODE_H
#define ANDNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

#include <fugio/core/variant_interface.h>

class AndNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Ands all the input booleans together." )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/And_Logic" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE AndNode( QSharedPointer<fugio::NodeInterface> pNode);

	virtual ~AndNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated(qint64 pTimeStamp);

	virtual QList<QUuid> pinAddTypesInput() const;

	virtual bool canAcceptPin(fugio::PinInterface *pPin) const;

protected:
	QSharedPointer<fugio::PinInterface>			 mPinOutput;
	fugio::VariantInterface						*mValOutput;
};

#endif // ANDNODE_H
