#ifndef NANDNODE_H
#define NANDNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

#include <fugio/core/variant_interface.h>

class NandNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Nands all the input booleans together." )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Nand_Logic" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE NandNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~NandNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

	virtual QList<QUuid> pinAddTypesInput() const;

	virtual bool canAcceptPin(fugio::PinInterface *pPin) const;

protected:
	QSharedPointer<fugio::PinInterface>			 mPinOutput;
	fugio::VariantInterface						*mValOutput;
};
#endif // NANDNODE_H
