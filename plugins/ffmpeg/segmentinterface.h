#ifndef INTERFACESEGMENT_H
#define INTERFACESEGMENT_H

#include <fugio/global.h>

#include <QString>
#include <QImage>
#include <QSharedPointer>

FUGIO_NAMESPACE_BEGIN

class SegmentInterface
{
public:
	typedef struct VidDat
	{
		qreal		 mPTS;
		qreal		 mDuration;
		int			 mLineSizes[ 8 ];
		bool		 mForced;
		quint8		*mData[ 8 ];

		VidDat( void )
			: mPTS( -1 ), mDuration( -1 ), mForced( false )
		{
			for( int i = 0 ; i < 8 ; i++ )
			{
				mLineSizes[ i ] = 0;
				mData[ i ] = nullptr;
			}
		}
	} VidDat;

	virtual ~SegmentInterface( void ) {}

	virtual bool loadMedia( const QString &pFileName, bool pProcess ) = 0;

	virtual qreal duration( void ) const = 0;

	virtual void setPlayhead( qreal pTimeStamp ) = 0;

	virtual void mixAudio( qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, void **pBuffers, float pVol ) const = 0;

	virtual qreal videoFrameTimeStamp( void ) const = 0;

	virtual qreal wavL( qreal pTimeStamp ) const = 0;
	virtual qreal wavR( qreal pTimeStamp ) const = 0;

	virtual QString filename( void ) const = 0;

	virtual bool hasVideo( void ) const = 0;
	virtual bool hasAudio( void ) const = 0;

	virtual const VidDat *viddat( void ) const = 0;

	virtual QSize imageSize( void ) const = 0;
	virtual int imageFormat( void ) const = 0;
	virtual bool imageIsHap( void ) const = 0;

	virtual void setPreload( bool pPreload ) = 0;

	virtual bool preload( void ) const = 0;

	virtual qreal videoFrameRate( void ) const = 0;

	virtual void readNext( void ) = 0;

	virtual int audioChannels( void ) const = 0;
	virtual qreal sampleRate( void ) const = 0;

	virtual QString statusMessage( void ) const = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::SegmentInterface, "com.bigfug.fugio.segment/1.0" )

#endif // INTERFACESEGMENT_H
