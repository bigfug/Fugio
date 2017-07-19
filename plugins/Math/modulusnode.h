#ifndef MODULUSNODE_H
#define MODULUSNODE_H

#include <fugio/nodecontrolbase.h>

#include <fugio/core/variant_interface.h>

class ModulusNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Modulus Operator" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Modulus" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE ModulusNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~ModulusNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputModulus;
	QSharedPointer<fugio::PinInterface>			 mPinInputNumber;

	QSharedPointer<fugio::PinInterface>			 mPinOutputNumber;
	fugio::VariantInterface						*mValOutputNumber;
};


#endif // MODULUSNODE_H
