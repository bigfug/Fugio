#ifndef DIVIDENODE_H
#define DIVIDENODE_H

#include <fugio/nodecontrolbase.h>

#include <fugio/core/array_interface.h>
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
		template<typename T> static T op0( const QList< QSharedPointer<fugio::PinInterface> > pInputPins );
		template<typename T> static T op1( const QList< QSharedPointer<fugio::PinInterface> > pInputPins );
		template<typename T> static T op2( const QList< QSharedPointer<fugio::PinInterface> > pInputPins );
		template<typename T> static T op3( const QList< QSharedPointer<fugio::PinInterface> > pInputPins );

		template<typename T, typename S = T> static void div( const QList<fugio::PinVariantIterator> &ItrLst, void *OutDst, int ItrMax );
		template<typename T, typename S = T> static void divs( const QList<fugio::PinVariantIterator> &ItrLst, void *OutDst, int ItrMax );
	};

public:
	Q_INVOKABLE DivideNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~DivideNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

	virtual QList<QUuid> pinAddTypesInput() const;

	virtual bool canAcceptPin(fugio::PinInterface *pPin) const;

private:
	static QVariant divideNumber( const QList< QSharedPointer<fugio::PinInterface> > pInputPins );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinNumerator;
	QSharedPointer<fugio::PinInterface>			 mPinDenominator;

	QSharedPointer<fugio::PinInterface>			 mPinOutputArray;
	fugio::ArrayInterface						*mValOutputArray;
};

#endif // DIVIDENODE_H
