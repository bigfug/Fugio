#ifndef BACKGROUNDSUBTRACTIONNODE_H
#define BACKGROUNDSUBTRACTIONNODE_H

#include <QObject>

#include <fugio/core/uuid.h>
#include <fugio/opencv/uuid.h>
#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/image/image.h>

#include <fugio/nodecontrolbase.h>

#if defined( OPENCV_SUPPORTED )
#include <opencv2/core/core.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/video/background_segm.hpp>
#endif

class BackgroundSubtractionNode : public fugio::NodeControlBase
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit BackgroundSubtractionNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~BackgroundSubtractionNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

private:
	static void process( BackgroundSubtractionNode *pNode );

	void process( void );

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputImage;
	QSharedPointer<fugio::PinInterface>			 mPinInputReset;

	QSharedPointer<fugio::PinInterface>			 mPinOutputImage;
	fugio::VariantInterface						*mValOutputImage;

#if defined( OPENCV_SUPPORTED )
	cv::Mat										 mMatSrc;
	cv::Mat										 mMatDst;
	cv::Ptr<cv::BackgroundSubtractor>			 mBckSub;
#endif
	double										 mLearningRate;
};

#endif // BACKGROUNDSUBTRACTIONNODE_H
