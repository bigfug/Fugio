#ifndef KEYFRAMEDATATIME_H
#define KEYFRAMEDATATIME_H

#include "keyframedata.h"

typedef enum TimeInterpolate
{
	TIME_PAUSE,
	TIME_LINEAR,
	TIME_FORWARD
} TimeInterpolate;

class KeyFrameDataTime : public KeyFrameData
{
public:
	KeyFrameDataTime( qreal pTimeStamp )
		: KeyFrameData( pTimeStamp )
	{

	}

	KeyFrameDataTime( qreal pTimeStamp, qreal pTime )
		: KeyFrameData( pTimeStamp ), mTime( pTime ), mInterpolation( TIME_FORWARD )
	{

	}

	KeyFrameDataTime( const KeyFrameDataTime &SRC )
		: KeyFrameData( SRC ), mTime( SRC.mTime ), mInterpolation( SRC.mInterpolation )
	{

	}

	inline qreal time( void ) const
	{
		return( mTime );
	}

	inline void setTime( qreal pTime )
	{
		mTime = pTime;
	}

	inline TimeInterpolate interpolation( void ) const
	{
		return( mInterpolation );
	}

	inline void setInterpolation( TimeInterpolate pInterpolation )
	{
		mInterpolation = pInterpolation;
	}

	virtual void cfgSave( QSettings &pDataStream ) const
	{
		KeyFrameData::cfgSave( pDataStream );

		pDataStream.setValue( "time", mTime );
		pDataStream.setValue( "interpolation", mInterpolation );
	}

	virtual void cfgLoad( QSettings &pDataStream )
	{
		KeyFrameData::cfgLoad( pDataStream );

		mTime = pDataStream.value( "time", mTime ).toDouble();

		mInterpolation = static_cast<TimeInterpolate>( pDataStream.value( "interpolation", mInterpolation ).toInt() );
	}

protected:
	qreal				mTime;
	TimeInterpolate		mInterpolation;
};

#endif // KEYFRAMEDATATIME_H
