#ifndef DEVICELEAP_H
#define DEVICELEAP_H

#include <QObject>
#include <QVector3D>

#if defined( LEAP_PLUGIN_SUPPORTED )

#include <Leap.h>

using namespace Leap;

#endif

class DeviceLeap : public QObject
#if defined( LEAP_PLUGIN_SUPPORTED )
		, public Listener
#endif
{
	Q_OBJECT

public:
	static void deviceInitialise( void );
	static void deviceDeinitialise( void );
//	static void devicePacketStart( qint64 pTimeStamp );
//	static void devicePacketEnd( void );

//	static void deviceCfgSave( QSettings &pDataStream );
//	static void deviceCfgLoad( QSettings &pDataStream );

	static DeviceLeap *newDevice( void );

	static void delDevice( DeviceLeap *pDelDev );

	static QList<DeviceLeap *> devices( void )
	{
		return( mDeviceList );
	}

private:
	explicit DeviceLeap( QObject *pParent = 0 );

	virtual ~DeviceLeap( void );

public:
//	virtual void packetStart( qreal pTimeStamp );
//	virtual void packetEnd( void );

//	virtual void cfgSave( QSettings &pDataStream ) const;
//	virtual void cfgLoad( QSettings &pDataStream );

//	virtual bool isEnabled( void ) const;
//	virtual void setEnabled( bool pEnabled );

	QVector3D stabilizedPalmPosition( void ) const
	{
		return( mStabilizedPalmPosition );
	}

	qint64 frameId( void ) const
	{
		return( mLastFrameId );
	}

	//-------------------------------------------------------------------------
	// Leap::Listener

#if defined( LEAP_PLUGIN_SUPPORTED )
	virtual void onInit( const Controller & );
	virtual void onConnect( const Controller & );
	virtual void onFrame( const Controller & );

	Controller &controller( void )
	{
		return( mController );
	}

	const Controller &controller( void ) const
	{
		return( mController );
	}

	Frame frame( void ) const
	{
		return( mCurrentFrame );
	}

	Hand &handLeft( void )
	{
		return( mHandLeft );
	}

	Hand &handRight( void )
	{
		return( mHandRight );
	}
#endif

private:
	static QList<DeviceLeap *>	 mDeviceList;

#if defined( LEAP_PLUGIN_SUPPORTED )
	Controller					 mController;
	Frame						 mCurrentFrame;
	Hand						 mHandLeft;
	Hand						 mHandRight;
#endif

	QVector3D					 mStabilizedPalmPosition;

	qint64						 mLastFrameId;
};

#endif // DEVICELEAP_H
