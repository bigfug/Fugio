#ifndef DEVICEVIDEOCAPTURE_H
#define DEVICEVIDEOCAPTURE_H

#include <QThread>
#include <QMutex>
#include <QVector>

#if defined( OPENCV_SUPPORTED )
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#endif

class DeviceVideoCapture : public QThread
{
	Q_OBJECT

public:
	static void deviceInitialise( void );
	static void deviceDeinitialise( void );
//	static void devicePacketStart( qint64 pTimeStamp );
//	static void devicePacketEnd( void );

//	static void deviceCfgSave( QSettings &pDataStream );
//	static void deviceCfgLoad( QSettings &pDataStream );

	static DeviceVideoCapture *newDevice( int pCameraIndex );

	static void delDevice( DeviceVideoCapture *pDelDev );

	static QVector<DeviceVideoCapture *> devices( void )
	{
		return( mDeviceList );
	}

protected:
	explicit DeviceVideoCapture( int pCameraIndex );

	virtual ~DeviceVideoCapture( void );

public:
	qint64 frameNumber( void ) const
	{
		return( mFrmNum );
	}

	int cameraIndex( void ) const
	{
		return( mCameraIndex );
	}

	void shutdown( void );

#if defined( OPENCV_SUPPORTED )
	cv::Mat lockFrame( void );

	void freeFrame( void );

	cv::Mat frame( void );
#endif

	// QThread interface
protected:
	virtual void run() Q_DECL_OVERRIDE;

signals:

public slots:

private:
	static QVector<DeviceVideoCapture *>	 mDeviceList;

	int										 mCameraIndex;
	QMutex									 mFrmMtx;
	volatile qint64							 mFrmNum;

#if defined( OPENCV_SUPPORTED )
	cv::VideoCapture						*mVidCap;
	cv::Mat									 mMatImg;
#endif
};

#endif // DEVICEVIDEOCAPTURE_H
