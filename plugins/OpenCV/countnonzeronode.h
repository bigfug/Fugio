#ifndef COUNTNONZERONODE_H
#define COUNTNONZERONODE_H

#include <fugio/nodecontrolbase.h>
#include <fugio/core/variant_interface.h>

class CountNonZeroNode : public fugio::NodeControlBase
{
	Q_OBJECT

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Count non zero pixels in a single plane image" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/CountNonZero_(OpenCV)" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit CountNonZeroNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~CountNonZeroNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp );

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputImage;

	QSharedPointer<fugio::PinInterface>			 mPinInputROI;

	QSharedPointer<fugio::PinInterface>			 mPinOutputCount;
	fugio::VariantInterface						*mValOutputCount;
};

#endif // COUNTNONZERONODE_H
