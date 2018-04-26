#ifndef SPLITRECTNODE_H
#define SPLITRECTNODE_H

#include <fugio/nodecontrolbase.h>
#include <fugio/core/variant_interface.h>

class SplitRectNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Splits a Rect node into its raw values." )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Split_(Rect)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE SplitRectNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~SplitRectNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputRect;

	QSharedPointer<fugio::PinInterface>			 mPinOutputPosition;
	QSharedPointer<fugio::PinInterface>			 mPinOutputSize;

	fugio::VariantInterface						*mValOutputPosition;
	fugio::VariantInterface						*mValOutputSize;
};

#endif // SPLITRECTNODE_H
