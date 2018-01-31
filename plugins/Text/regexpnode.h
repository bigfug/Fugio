#ifndef REGEXPNODE_H
#define REGEXPNODE_H

#include <QObject>
#include <QRegExp>

#include <fugio/node_interface.h>
#include <fugio/pin_interface.h>

#include <fugio/core/variant_interface.h>

#include <fugio/core/uuid.h>

#include <fugio/nodecontrolbase.h>

class RegExpNode : public fugio::NodeControlBase
{
	Q_OBJECT

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Matches patterns in a string using Regular Expressions" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Regexp" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit RegExpNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~RegExpNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp );

private:
	QSharedPointer<fugio::PinInterface>			 mPinInput;

	QSharedPointer<fugio::PinInterface>			 mPinRegExp;

	QSharedPointer<fugio::PinInterface>			 mPinOutputMatches;
	fugio::VariantInterface						*mValOutputMatches;

	QSharedPointer<fugio::PinInterface>			 mPinOutputExactMatch;
	fugio::VariantInterface						*mValOutputExactMatch;
};

#endif // REGEXPNODE_H
