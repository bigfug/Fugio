#ifndef ADDNODE_H
#define ADDNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

#include <fugio/core/variant_interface.h>

#include <fugio/pin_variant_iterator.h>

class AddNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Adds all the input numbers together." )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Add" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

	class Operator
	{
	public:
		template<typename T> static void add( const QList<fugio::PinVariantIterator> &ItrLst, fugio::VariantInterface *OutDst, int ItrMax );
	};

public:
	Q_INVOKABLE AddNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~AddNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

	virtual QList<QUuid> pinAddTypesInput() const;
	virtual bool canAcceptPin(fugio::PinInterface *pPin) const;

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInput;
	QSharedPointer<fugio::PinInterface>			 mPinOutput;

	fugio::VariantInterface						*mValOutputArray;
};

#endif // ADDNODE_H
