#ifndef LEAPHANDPIN_H
#define LEAPHANDPIN_H

#include <fugio/core/uuid.h>

#include <QObject>

#if defined( LEAP_PLUGIN_SUPPORTED )
#include <Leap.h>
#endif

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/leapmotion/hand_interface.h>

#include <fugio/pincontrolbase.h>

class LeapHandPin : public fugio::PinControlBase, public fugio::HandInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::HandInterface )

public:
	Q_INVOKABLE explicit LeapHandPin( QSharedPointer<fugio::PinInterface> pPin )
		: PinControlBase( pPin )
	{

	}

	virtual ~LeapHandPin( void ) {}

#if defined( LEAP_PLUGIN_SUPPORTED )
	void setHand( Leap::Hand &pHand )
	{
		mHand = pHand;
	}
#endif

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const;

	virtual QString description( void ) const
	{
		return( "Leap Hand" );
	}

	//-------------------------------------------------------------------------
	// InterfaceLeapHand

#if defined( LEAP_PLUGIN_SUPPORTED )
	virtual const Leap::Hand &hand( void ) const
	{
		return( mHand );
	}
#endif

private:
#if defined( LEAP_PLUGIN_SUPPORTED )
	Leap::Hand			 mHand;
#endif
};

#endif // LEAPHANDPIN_H
