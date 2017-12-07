#ifndef SIMPLEBLOBDETECTORNODE_H
#define SIMPLEBLOBDETECTORNODE_H

#include <QObject>

#include <fugio/image/image_interface.h>
#include <fugio/core/array_interface.h>

#include <fugio/nodecontrolbase.h>

#if defined( OPENCV_SUPPORTED )
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#endif

class SimpleBlobDetectorNode : public fugio::NodeControlBase
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit SimpleBlobDetectorNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~SimpleBlobDetectorNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp );

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputImage;

	QSharedPointer<fugio::PinInterface>			 mPinOutputPoints;
	fugio::ArrayInterface						*mValOutputPoints;

	QSharedPointer<fugio::PinInterface>			 mPinOutputSizes;
	fugio::ArrayInterface						*mValOutputSizes;

	QVector<QPointF>							 mPoints;
	QVector<float>								 mSizes;

#if defined( OPENCV_SUPPORTED )
	cv::Mat										 mMatImg;
	cv::Ptr<cv::SimpleBlobDetector>				 mDetector;
#endif
};

#endif // SIMPLEBLOBDETECTORNODE_H
