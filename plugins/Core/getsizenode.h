#ifndef GETSIZENODE_H
#define GETSIZENODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>
#include <fugio/paired_pins_helper_interface.h>

class GetSizeNode : public fugio::NodeControlBase, fugio::PairedPinsHelperInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::PairedPinsHelperInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Gets the size of the input" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Get_Size" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE GetSizeNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~GetSizeNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual QList<QUuid> pinAddTypesInput() const Q_DECL_OVERRIDE;

	virtual bool canAcceptPin( fugio::PinInterface *pPin ) const Q_DECL_OVERRIDE;

	virtual bool pinShouldAutoRename( fugio::PinInterface *pPin ) const Q_DECL_OVERRIDE;

	// PairedPinsHelperInterface interface
public:
	virtual QUuid pairedPinControlUuid( QSharedPointer<fugio::PinInterface> pPin ) const Q_DECL_OVERRIDE;
};

#endif // GETSIZENODE_H
