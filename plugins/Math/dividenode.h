#ifndef DIVIDENODE_H
#define DIVIDENODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

#include <fugio/core/variant_interface.h>

class DivideNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Divides one number by another." )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Divide_Number" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE DivideNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~DivideNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinNumerator;
	QSharedPointer<fugio::PinInterface>			 mPinDenominator;

	QSharedPointer<fugio::PinInterface>			 mPinOutput;
	fugio::VariantInterface						*mValOutput;
};

#endif // DIVIDENODE_H
