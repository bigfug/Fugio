#ifndef BITSTOINTNODE_H
#define BITSTOINTNODE_H

#include <fugio/nodecontrolbase.h>

#include <fugio/core/variant_interface.h>

#include <fugio/pin_variant_iterator.h>

#include <fugio/paired_pins_helper_interface.h>

class BitsToIntNode : public fugio::NodeControlBase, public fugio::PairedPinsHelperInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::PairedPinsHelperInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "BitsToInt" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE BitsToIntNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~BitsToIntNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual QList<QUuid> pinAddTypesInput() const Q_DECL_OVERRIDE;

	virtual bool canAcceptPin(fugio::PinInterface *pPin) const Q_DECL_OVERRIDE;

	// PairedPinsHelperInterface interface
public:
	virtual QUuid pairedPinControlUuid( QSharedPointer<fugio::PinInterface> pPin ) const Q_DECL_OVERRIDE;
};


#endif // BITSTOINTNODE_H
