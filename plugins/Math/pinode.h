#ifndef PINODE_H
#define PINODE_H

#include <fugio/nodecontrolbase.h>

#include <fugio/core/variant_interface.h>

class PiNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "The mathematical constant Pi" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Pi" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE PiNode( QSharedPointer<fugio::NodeInterface> pNode);

	virtual ~PiNode( void ) {}

protected:
	QSharedPointer<fugio::PinInterface>			 mPinOutput;
	fugio::VariantInterface						*mValOutput;
};

#endif // PINODE_H
