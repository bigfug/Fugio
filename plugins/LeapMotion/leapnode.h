#ifndef LEAPNODE_H
#define LEAPNODE_H

#include <fugio/leapmotion/uuid.h>
#include <fugio/nodecontrolbase.h>
#include <fugio/pin_interface.h>
#include <fugio/core/variant_interface.h>
#include <fugio/image/image.h>

#include "leaphandpin.h"

class DeviceLeap;

class LeapNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Gets data from the Leap Motion sensor." )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Leap" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit LeapNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~LeapNode( void ) {}

	//-------------------------------------------------------------------------
	// InterfaceNodeControl

	virtual bool initialise( void );

	virtual bool deinitialise( void );

protected slots:
	void onContextFrameStart( qint64 pTimeStamp );

private:
	DeviceLeap							*mDevice;
	int64_t								 mLastFrameId;

	QSharedPointer<fugio::PinInterface>		 mPinHandLeft;
	LeapHandPin							*mHandLeft;

	QSharedPointer<fugio::PinInterface>		 mPinHandRight;
	LeapHandPin							*mHandRight;

	QSharedPointer<fugio::PinInterface>		 mPinImage1;
	fugio::VariantInterface						*mImage1;

	QSharedPointer<fugio::PinInterface>		 mPinDistortionImage;
	fugio::VariantInterface						*mDistortionImage;
};

#endif // LEAPNODE_H
