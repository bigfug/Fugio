#ifndef OSCSPLITNODE_H
#define OSCSPLITNODE_H

#include <fugio/nodecontrolbase.h>
#include <fugio/osc/uuid.h>
#include <fugio/osc/split_interface.h>
#include <fugio/osc/namespace_interface.h>

class SplitNode : public fugio::NodeControlBase, public fugio::osc::SplitInterface, public fugio::osc::NamespaceInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::osc::SplitInterface fugio::osc::NamespaceInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "OSC_Split" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit SplitNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~SplitNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual QList<NodeControlInterface::AvailablePinEntry> availableOutputPins( void ) const Q_DECL_OVERRIDE;

	virtual QList<QUuid> pinAddTypesOutput() const Q_DECL_OVERRIDE;

	virtual QUuid pinAddControlUuid(fugio::PinInterface *pPin) const Q_DECL_OVERRIDE;

	//-------------------------------------------------------------------------
	// SplitInterface interface

	virtual void oscSplit( QStringList pPath, const QVariant &pValue ) Q_DECL_OVERRIDE;

	virtual void oscPath( QStringList &pPath ) const Q_DECL_OVERRIDE;

	//-------------------------------------------------------------------------
	// NamespaceInterface interface
public:
	virtual QStringList oscNamespace() Q_DECL_OVERRIDE;

	virtual QList<NodeControlInterface::AvailablePinEntry> oscPins( const QStringList &pCurDir ) const Q_DECL_OVERRIDE;

private:
	QSharedPointer<fugio::PinInterface>			mPinInput;
};

#endif // OSCSPLITNODE_H
