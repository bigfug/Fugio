#ifndef COMPARESTRINGNODE_H
#define COMPARESTRINGNODE_H

#include <fugio/nodecontrolbase.h>

#include <fugio/paired_pins_helper_interface.h>

#include <fugio/core/variant_interface.h>

class CompareStringNode : public fugio::NodeControlBase, public fugio::PairedPinsHelperInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::PairedPinsHelperInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Compares strings" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Compare_String" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit CompareStringNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~CompareStringNode( void ) Q_DECL_OVERRIDE {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual QList<QUuid> pinAddTypesInput() const Q_DECL_OVERRIDE;

	virtual bool canAcceptPin( fugio::PinInterface *pPin ) const Q_DECL_OVERRIDE;

	//-------------------------------------------------------------------------
	// PairedPinsHelperInterface interface

	virtual QUuid pairedPinControlUuid( QSharedPointer<fugio::PinInterface> pPin ) const Q_DECL_OVERRIDE;

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputCompare;

	QSharedPointer<fugio::PinInterface>			 mPinInputCaseSensitive;

	QSharedPointer<fugio::PinInterface>			 mPinInputSource;

	QSharedPointer<fugio::PinInterface>			 mPinOutputResult;
	fugio::VariantInterface						*mValOutputResult;
};

#endif // COMPARESTRINGNODE_H
