#ifndef SPLITSIZENODE_H
#define SPLITSIZENODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>
#include <fugio/core/variant_interface.h>

class SplitSizeNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Splits a Size node into its raw values." )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Split_(Size)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE SplitSizeNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~SplitSizeNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInput;

	QSharedPointer<fugio::PinInterface>			 mPinWidth;
	QSharedPointer<fugio::PinInterface>			 mPinHeight;

	fugio::VariantInterface						*mWidth;
	fugio::VariantInterface						*mHeight;
};

#endif // SPLITSIZENODE_H
