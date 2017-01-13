#ifndef NUMBERRANGELIMITNODE_H
#define NUMBERRANGELIMITNODE_H

#include <fugio/nodecontrolbase.h>
#include <fugio/core/uuid.h>
#include <fugio/core/variant_interface.h>
#include <fugio/pin_interface.h>

class NumberRangeLimitNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Limits an input number to a specific range." )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Range_Limit_(Number)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit NumberRangeLimitNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~NumberRangeLimitNode( void );

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp );

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputValue;

	QSharedPointer<fugio::PinInterface>			 mPinInputMinInput;
	QSharedPointer<fugio::PinInterface>			 mPinInputMaxInput;

	QSharedPointer<fugio::PinInterface>			 mPinOutputValue;
	fugio::VariantInterface						*mValue;
};

#endif // NUMBERRANGELIMITNODE_H
