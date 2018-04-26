#ifndef DISTANCETRANSFORMNODE_H
#define DISTANCETRANSFORMNODE_H

#include <QObject>

#include <fugio/core/uuid.h>
#include <fugio/opencv/uuid.h>
#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/image/image.h>

#include <fugio/nodecontrolbase.h>

#if defined( OPENCV_SUPPORTED )
#include <opencv2/core/core.hpp>
#endif

class DistanceTransformNode : public fugio::NodeControlBase
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit DistanceTransformNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~DistanceTransformNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp );

private:
	static void conversion( DistanceTransformNode *pNode );

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputImage;

	QSharedPointer<fugio::PinInterface>			 mPinOutputImage;
	fugio::VariantInterface						*mValOutputImage;

	QSharedPointer<fugio::PinInterface>			 mPinOutputLabels;
	fugio::VariantInterface						*mValOutputLabels;

#if defined( OPENCV_SUPPORTED )
	cv::Mat										 mMatImg;
	cv::Mat										 mMatLab;
#endif
};

#endif // DISTANCETRANSFORMNODE_H
