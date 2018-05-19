#ifndef STRINGTOJSONNODE_H
#define STRINGTOJSONNODE_H

#include <fugio/nodecontrolbase.h>
#include <fugio/core/variant_interface.h>
#include <fugio/text/syntax_error_interface.h>

#include <QJsonDocument>
#include <QJsonArray>

class StringToJsonNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "String to JSON" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "String_To_Json" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE StringToJsonNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~StringToJsonNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputString;
	fugio::SyntaxErrorInterface				*mValInputSource;

	QSharedPointer<fugio::PinInterface>			 mPinOutputJson;
	fugio::VariantInterface						*mValOutputJson;
};


#endif // STRINGTOJSONNODE_H
