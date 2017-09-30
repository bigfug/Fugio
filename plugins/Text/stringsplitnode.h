#ifndef STRINGSPLITNODE_H
#define STRINGSPLITNODE_H

#include <QObject>

#include <fugio/core/uuid.h>
#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/core/variant_interface.h>
#include <fugio/nodecontrolbase.h>

class StringSplitNode : public fugio::NodeControlBase
{
	Q_OBJECT

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Splits strings" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Split_(String)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit StringSplitNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~StringSplitNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputString;
	QSharedPointer<fugio::PinInterface>			 mPinInputSplit;

	QSharedPointer<fugio::PinInterface>			 mPinOutputList;
	fugio::VariantInterface						*mValOutputList;
};

#endif // STRINGSPLITNODE_H
