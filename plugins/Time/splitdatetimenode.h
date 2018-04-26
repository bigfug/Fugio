#ifndef SPLITDATETIMENODE_H
#define SPLITDATETIMENODE_H

#include <fugio/nodecontrolbase.h>

#include <fugio/core/variant_interface.h>

class SplitDateTimeNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Splits a DateTime into a Date and a Time" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "SplitDateTime" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit SplitDateTimeNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~SplitDateTimeNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp );

private:
	QSharedPointer<fugio::PinInterface>	 mPinInputDateTime;

	QSharedPointer<fugio::PinInterface>	 mPinOutputDate;
	fugio::VariantInterface				*mValOutputDate;

	QSharedPointer<fugio::PinInterface>	 mPinOutputTime;
	fugio::VariantInterface				*mValOutputTime;
};

#endif // SPLITDATETIMENODE_H
