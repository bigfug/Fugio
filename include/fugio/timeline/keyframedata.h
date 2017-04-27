#ifndef KEYFRAMEDATA_H
#define KEYFRAMEDATA_H

#include <QtGlobal>
#include <QSettings>

class KeyFrameData
{
public:
	KeyFrameData( qreal pTimeStamp )
		: mTimeStamp( pTimeStamp )
	{

	}

	KeyFrameData( const KeyFrameData &KFD )
		: mTimeStamp( KFD.mTimeStamp )
	{

	}

    virtual ~KeyFrameData( void )
    {

    }

	inline virtual void setTimeStamp( qreal pTimeStamp )
	{
		mTimeStamp = pTimeStamp;
	}

	inline virtual qreal timestamp( void ) const
	{
		return( mTimeStamp );
	}

	virtual void cfgSave( QSettings &pDataStream ) const
	{
		pDataStream.setValue( "timestamp", mTimeStamp );
	}

	virtual void cfgLoad( QSettings &pDataStream )
	{
		mTimeStamp = pDataStream.value( "timestamp", mTimeStamp ).toDouble();
	}

protected:
	qreal		mTimeStamp;
};

#endif // KEYFRAMEDATA_H
