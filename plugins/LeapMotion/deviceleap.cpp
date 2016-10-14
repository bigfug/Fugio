#include "deviceleap.h"

#include <QMessageBox>
#include <QSettings>
#include <QtEndian>
#include <QColor>
#include <QDebug>

#include <fugio/global_interface.h>

QList<DeviceLeap *>		 DeviceLeap::mDeviceList;

void DeviceLeap::deviceInitialise( void )
{

}

void DeviceLeap::deviceDeinitialise( void )
{
}

DeviceLeap *DeviceLeap::newDevice()
{
	if( mDeviceList.isEmpty() )
	{
		DeviceLeap		*NewDev = new DeviceLeap();

		if( NewDev != 0 )
		{
			mDeviceList.append( NewDev );
		}
	}

	return( mDeviceList.isEmpty() ? 0 : mDeviceList.first() );
}

void DeviceLeap::delDevice( DeviceLeap *pDelDev )
{
	Q_UNUSED( pDelDev )
}

DeviceLeap::DeviceLeap( QObject *pParent ) :
	QObject( pParent ), mLastFrameId( 0 )
{
#if defined( LEAP_PLUGIN_SUPPORTED )
	mController.setPolicy( Leap::Controller::POLICY_IMAGES );

	mController.addListener( *this );
#endif
}

DeviceLeap::~DeviceLeap( void )
{
#if defined( LEAP_PLUGIN_SUPPORTED )
	mController.removeListener( *this );
#endif
}

#if defined( LEAP_PLUGIN_SUPPORTED )

void DeviceLeap::onInit( const Controller &pController )
{
	Q_UNUSED( pController )
}

void DeviceLeap::onConnect( const Controller &pController )
{
	Q_UNUSED( pController )
}

void DeviceLeap::onFrame( const Controller &pController )
{
	mCurrentFrame = pController.frame();

	HandList hands = mCurrentFrame.hands();

	for( HandList::const_iterator hl = hands.begin() ; hl != hands.end() ; ++hl )
	{
		const Hand hand = *hl;

		if( !hand.isValid() )
		{
			continue;
		}

		if( hand.isRight() )
		{
			mHandRight = hand;
		}

		if( hand.isLeft() )
		{
			mHandLeft = hand;
		}
	}

	mLastFrameId = mCurrentFrame.id();
}

#endif
