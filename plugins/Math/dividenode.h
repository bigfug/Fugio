#ifndef DIVIDENODE_H
#define DIVIDENODE_H

#include <fugio/nodecontrolbase.h>

#include <fugio/pin_variant_iterator.h>

class DivideNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Divides one number by another." )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Divide" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

	class Operator
	{
	public:
		template<typename T, typename S = T> static void div( const QList<fugio::PinVariantIterator> &ItrLst, fugio::VariantInterface *OutDst, int ItrMax );
		template<typename T, typename S = T> static void divs( const QList<fugio::PinVariantIterator> &ItrLst, fugio::VariantInterface *OutDst, int ItrMax );
	};

public:
	Q_INVOKABLE DivideNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~DivideNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

	virtual QList<QUuid> pinAddTypesInput() const;

	virtual bool canAcceptPin(fugio::PinInterface *pPin) const;

protected:
	QSharedPointer<fugio::PinInterface>			 mPinNumerator;
	QSharedPointer<fugio::PinInterface>			 mPinDenominator;

	QSharedPointer<fugio::PinInterface>			 mPinOutputArray;
	fugio::VariantInterface						*mValOutputArray;
};

#endif // DIVIDENODE_H
