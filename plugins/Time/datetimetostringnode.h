#ifndef DateTimeToStringNode_H
#define DateTimeToStringNode_H

#include <fugio/nodecontrolbase.h>

#include <fugio/core/variant_interface.h>

class DateTimeToStringNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Converts date, time, and datetime to string" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "DateTimeToString" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit DateTimeToStringNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~DateTimeToStringNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputInput;
	QSharedPointer<fugio::PinInterface>			 mPinInputFormat;

	QSharedPointer<fugio::PinInterface>			 mPinOutputString;
	fugio::VariantInterface						*mValOutputString;
};

#endif // DateTimeToStringNode_H
