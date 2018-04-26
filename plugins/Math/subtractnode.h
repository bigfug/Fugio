#ifndef SUBTRACTNODE_H
#define SUBTRACTNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

#include <fugio/pin_variant_iterator.h>

class SubtractNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Subtract" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Subtract" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

	class Operator
	{
	public:
		template<typename T> static void sub( const QList<fugio::PinVariantIterator> &ItrLst, fugio::VariantInterface *OutDst, int ItrMax );
	};

public:
	Q_INVOKABLE SubtractNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~SubtractNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

	virtual QList<QUuid> pinAddTypesInput() const;

	virtual bool canAcceptPin(fugio::PinInterface *pPin) const;

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInput;

	QSharedPointer<fugio::PinInterface>			 mPinOutput;
	fugio::VariantInterface						*mValOutputArray;
};

#endif // SUBTRACTNODE_H
