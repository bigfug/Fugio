#ifndef VIDEOCAPTURENODE_H
#define VIDEOCAPTURENODE_H

#include <QObject>
#include <QPointer>

#include <fugio/core/uuid.h>
#include <fugio/opencv/uuid.h>
#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/image/image.h>
#include <fugio/core/variant_interface.h>

#include <fugio/nodecontrolbase.h>

#include "devicevideocapture.h"

class VideoCaptureNode : public fugio::NodeControlBase
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit VideoCaptureNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~VideoCaptureNode( void );

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual bool initialise( void ) Q_DECL_OVERRIDE;

	virtual bool deinitialise( void ) Q_DECL_OVERRIDE;

	virtual QWidget *gui() Q_DECL_OVERRIDE;

private slots:
	void onFrameStart( qint64 pTimeStamp );

	void setCameraIndex( int pIndex );

private:
	QSharedPointer<fugio::PinInterface>		 mPinOutputImage;
	fugio::VariantInterface							*mOutputImage;

	QSharedPointer<fugio::PinInterface>		 mPinOutputSize;
	fugio::VariantInterface					*mOutputSize;

	DeviceVideoCapture						*mVidCapDev;
	qint64									 mFrmNum;

#if defined( OPENCV_SUPPORTED )
	cv::Mat									 mMatImg;
#endif

	int										 mCameraIndex;
};

#endif // VIDEOCAPTURENODE_H
