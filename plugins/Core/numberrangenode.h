#ifndef NUMBERRANGENODE_H
#define NUMBERRANGENODE_H

#include <fugio/nodecontrolbase.h>
#include <fugio/core/uuid.h>
#include <fugio/core/variant_interface.h>
#include <fugio/pin_interface.h>

class NumberRangeNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Maps a number from an input range to an output range." )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Range_Map_(Number)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit NumberRangeNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~NumberRangeNode( void );

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp );

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputValue;

	QSharedPointer<fugio::PinInterface>			 mPinInputMinInput;
	QSharedPointer<fugio::PinInterface>			 mPinInputMaxInput;
	QSharedPointer<fugio::PinInterface>			 mPinInputMinOutput;
	QSharedPointer<fugio::PinInterface>			 mPinInputMaxOutput;

	QSharedPointer<fugio::PinInterface>			 mPinOutputValue;
	fugio::VariantInterface						*mValue;
};

#endif // NUMBERRANGENODE_H
