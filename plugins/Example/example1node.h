#ifndef EXAMPLENODE1_H
#define EXAMPLENODE1_H

#include <QObject>

#include <fugio/nodecontrolbase.h>
#include <fugio/core/variant_interface.h>

class Example1Node : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "First Example" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Example1" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE Example1Node( QSharedPointer<fugio::NodeInterface> pNode);

	virtual ~Example1Node( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInput;

	QSharedPointer<fugio::PinInterface>			 mPinOutput;

	fugio::VariantInterface						*mOutput;
};


#endif // EXAMPLENODE1_H
