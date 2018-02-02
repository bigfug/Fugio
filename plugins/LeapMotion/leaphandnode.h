#ifndef LEAPHANDNODE_H
#define LEAPHANDNODE_H

#include <fugio/leapmotion/uuid.h>
#include <fugio/nodecontrolbase.h>
#include <fugio/pin_interface.h>
#include <fugio/core/variant_interface.h>
#include <fugio/image/image.h>

class DeviceLeap;

class LeapHandNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Gets data from the Leap Motion sensor." )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Hand_(Leap)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit LeapHandNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~LeapHandNode( void ) {}

	//-------------------------------------------------------------------------
	// InterfaceNodeControl interface
public:
	virtual void inputsUpdated( qint64 pTimeStamp );

private:
	QSharedPointer<fugio::PinInterface>		 mPinHand;

	QSharedPointer<fugio::PinInterface>		 mOutputStabilizedPalmPositionPin;
	fugio::VariantInterface					*mOutputStabilizedPalmPosition;

	QSharedPointer<fugio::PinInterface>		 mPinNormal;
	fugio::VariantInterface					*mNormal;

	QSharedPointer<fugio::PinInterface>		 mPinDirection;
	fugio::VariantInterface					*mDirection;

	QSharedPointer<fugio::PinInterface>		 mPinPinchStrength;
	fugio::VariantInterface					*mPinchStrength;

	QSharedPointer<fugio::PinInterface>		 mPinGrabStrength;
	fugio::VariantInterface					*mGrabStrength;
};

#endif // LEAPHANDNODE_H
