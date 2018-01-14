#ifndef NUMBERARRAYNODE_H
#define NUMBERARRAYNODE_H

#include <fugio/nodecontrolbase.h>

#include <fugio/core/variant_interface.h>

class NumberArrayNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Number_Array" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE NumberArrayNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~NumberArrayNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputStart;
	QSharedPointer<fugio::PinInterface>			 mPinInputEnd;
	QSharedPointer<fugio::PinInterface>			 mPinInputIncrement;

	QSharedPointer<fugio::PinInterface>			 mPinOutputArray;
	fugio::VariantInterface						*mValOutputArray;
};

#endif // NUMBERARRAYNODE_H
