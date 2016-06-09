#ifndef SINRADIANNODE_H
#define SINRADIANNODE_H

#include <fugio/nodecontrolbase.h>

#include <fugio/core/variant_interface.h>

class SinRadianNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Calculates sin() from angle in radians" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Sin_Radian" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE SinRadianNode( QSharedPointer<fugio::NodeInterface> pNode);

	virtual ~SinRadianNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated(qint64 pTimeStamp);

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInput;

	QSharedPointer<fugio::PinInterface>			 mPinOutput;
	fugio::VariantInterface						*mValOutput;
};

#endif // SINRADIANNODE_H
