#ifndef SINDEGREENODE_H
#define SINDEGREENODE_H

#include <fugio/nodecontrolbase.h>

#include <fugio/core/variant_interface.h>

class SinDegreeNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Calculates sin() from angle in degrees" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Sine_(Degree)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE SinDegreeNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~SinDegreeNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInput;

	QSharedPointer<fugio::PinInterface>			 mPinOutput;
	fugio::VariantInterface						*mValOutput;
};

#endif // SINDEGREENODE_H
