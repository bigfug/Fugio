#ifndef STRINGTOFLOATNODE_H
#define STRINGTOFLOATNODE_H

#include <fugio/nodecontrolbase.h>
#include <fugio/core/variant_interface.h>

class StringToFloatNode : public fugio::NodeControlBase
{
	Q_OBJECT

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Converts a string to a floating point number" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "StringToFloat" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit StringToFloatNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~StringToFloatNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp );

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputString;

	QSharedPointer<fugio::PinInterface>			 mPinOutputValue;
	fugio::VariantInterface						*mValOutputValue;
};

#endif // STRINGTOFLOATNODE_H
