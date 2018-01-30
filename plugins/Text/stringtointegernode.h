#ifndef STRINGTOINTEGERNODE_H
#define STRINGTOINTEGERNODE_H

#include <fugio/nodecontrolbase.h>
#include <fugio/core/variant_interface.h>

class StringToIntegerNode : public fugio::NodeControlBase
{
	Q_OBJECT

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Converts a string to a integer number" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "StringToInteger" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit StringToIntegerNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~StringToIntegerNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp );

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputString;

	QSharedPointer<fugio::PinInterface>			 mPinOutputValue;
	fugio::VariantInterface						*mValOutputValue;
};

#endif // STRINGTOINTEGERNODE_H
