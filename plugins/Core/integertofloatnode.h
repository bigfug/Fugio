#ifndef INTEGERTOFLOATNODE_H
#define INTEGERTOFLOATNODE_H

#include <fugio/nodecontrolbase.h>
#include <fugio/paired_pins_helper_interface.h>

class IntegerToFloatNode : public fugio::NodeControlBase, public fugio::PairedPinsHelperInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::PairedPinsHelperInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Converts integers to floats" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Integer_To_Float" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE IntegerToFloatNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~IntegerToFloatNode( void ) {}


	// NodeControlInterface interface
public:
	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual QList<QUuid> pinAddTypesInput() const Q_DECL_OVERRIDE;

	virtual bool canAcceptPin( fugio::PinInterface *pPin ) const Q_DECL_OVERRIDE;

	virtual bool pinShouldAutoRename( fugio::PinInterface *pPin ) const Q_DECL_OVERRIDE;

	// PairedPinsHelperInterface interface
public:
	virtual QUuid pairedPinControlUuid(QSharedPointer<fugio::PinInterface> pPin) const Q_DECL_OVERRIDE;
};

#endif // INTEGERTOFLOATNODE_H
