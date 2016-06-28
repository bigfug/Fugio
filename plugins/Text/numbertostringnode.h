#ifndef NUMBERTOSTRING_H
#define NUMBERTOSTRING_H

#include <QObject>

#include <fugio/core/uuid.h>
#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/core/variant_interface.h>
#include <fugio/nodecontrolbase.h>

class NumberToStringNode : public fugio::NodeControlBase
{
	Q_OBJECT

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Converts a number to a string with control over width, base, and padding" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Number_To_String" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit NumberToStringNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~NumberToStringNode( void )
	{

	}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp );

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputNumber;
	QSharedPointer<fugio::PinInterface>			 mPinInputBase;
	QSharedPointer<fugio::PinInterface>			 mPinInputWidth;
	QSharedPointer<fugio::PinInterface>			 mPinInputCharacter;

	QSharedPointer<fugio::PinInterface>			 mPinOutput;
	fugio::VariantInterface						*mPinOutputValue;
};

#endif // NUMBERTOSTRING_H
