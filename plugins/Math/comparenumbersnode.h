#ifndef COMPARENUMBERSNODE_H
#define COMPARENUMBERSNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

#include <fugio/core/variant_interface.h>

class CompareNumbersNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Compares two numbers" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Compare_Number" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE CompareNumbersNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~CompareNumbersNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInput1;
	QSharedPointer<fugio::PinInterface>			 mPinInput2;

	QSharedPointer<fugio::PinInterface>			 mPinEqual;
	fugio::VariantInterface						*mValEqual;

	QSharedPointer<fugio::PinInterface>			 mPinFuzzy;
	fugio::VariantInterface						*mValFuzzy;

	QSharedPointer<fugio::PinInterface>			 mPinHigher;
	fugio::VariantInterface						*mValHigher;
};

#endif // COMPARENUMBERSNODE_H
