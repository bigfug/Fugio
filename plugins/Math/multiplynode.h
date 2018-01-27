#ifndef MULTIPLYNODE_H
#define MULTIPLYNODE_H

#include <QPoint>
#include <QPointF>

#include <fugio/nodecontrolbase.h>

#include <fugio/pin_variant_iterator.h>

class MultiplyNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Multiplies all the input numbers together." )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Multiply" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

	class Operator
	{
	public:
		template<typename T> static T op0( const QList< QSharedPointer<fugio::PinInterface> > pInputPins );
		template<typename T> static T op1( const QList< QSharedPointer<fugio::PinInterface> > pInputPins );
		template<typename T> static T op2( const QList< QSharedPointer<fugio::PinInterface> > pInputPins );
		template<typename T> static T op3( const QList< QSharedPointer<fugio::PinInterface> > pInputPins );

		template<typename T, typename S = T> static void mul( const QList<fugio::PinVariantIterator> &ItrLst, fugio::VariantInterface *OutDst, int ItrMax );
		template<typename T, typename S = T> static void muls( const QList<fugio::PinVariantIterator> &ItrLst, fugio::VariantInterface *OutDst, int ItrMax );
	};

public:
	Q_INVOKABLE MultiplyNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~MultiplyNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

	virtual QList<QUuid> pinAddTypesInput() const;

	virtual bool canAcceptPin(fugio::PinInterface *pPin) const;

private:
	static QVariant multiplyNumber( const QList< QSharedPointer<fugio::PinInterface> > pInputPins );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInput;

	QSharedPointer<fugio::PinInterface>			 mPinOutputArray;
	fugio::VariantInterface						*mValOutputArray;
};

#endif // MULTIPLYNODE_H
