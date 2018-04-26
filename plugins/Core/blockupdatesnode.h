#ifndef BLOCKUPDATESNODE_H
#define BLOCKUPDATESNODE_H

#include <fugio/nodecontrolbase.h>

#include <fugio/core/variant_interface.h>

#include <fugio/paired_pins_helper_interface.h>

class BlockUpdatesNode : public fugio::NodeControlBase, public fugio::PairedPinsHelperInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::PairedPinsHelperInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Block_Updates" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE BlockUpdatesNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~BlockUpdatesNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual bool canAcceptPin( fugio::PinInterface *pPin ) const Q_DECL_OVERRIDE;

	// PairedPinsHelperInterface interface
public:
	virtual QUuid pairedPinControlUuid( QSharedPointer<fugio::PinInterface> pPin ) const Q_DECL_OVERRIDE;

protected slots:
	void pinLinked( QSharedPointer<fugio::PinInterface> P1, QSharedPointer<fugio::PinInterface> P2 );
	void pinUnlinked( QSharedPointer<fugio::PinInterface> P1, QSharedPointer<fugio::PinInterface> P2 );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputBlock;

	QSharedPointer<fugio::PinInterface>			 mPinInput;

	QSharedPointer<fugio::PinInterface>			 mPinOutput;
	fugio::VariantInterface						*mValOutput;
};

#endif // BLOCKUPDATESNODE_H
