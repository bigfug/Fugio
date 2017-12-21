#ifndef COSINEDEGREESNODE_H
#define COSINEDEGREESNODE_H


#include <fugio/nodecontrolbase.h>

#include <fugio/core/variant_interface.h>

class CosineDegreeNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Calculates cos() from angle in degrees" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Cosine_(Degree)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE CosineDegreeNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~CosineDegreeNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInput;

	QSharedPointer<fugio::PinInterface>			 mPinOutput;
	fugio::VariantInterface						*mValOutput;
};

#endif // COSINEDEGREESNODE_H
