#ifndef PARSEJSONNODE_H
#define PARSEJSONNODE_H

#include <fugio/nodecontrolbase.h>

class ParseJsonNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", tr( "" ) )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Parse_Json" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE ParseJsonNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~ParseJsonNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputJson;

	QSharedPointer<fugio::PinInterface>			 mPinOutputJson;
	fugio::VariantInterface						*mValOutputJson;
};


#endif // PARSEJSONNODE_H
