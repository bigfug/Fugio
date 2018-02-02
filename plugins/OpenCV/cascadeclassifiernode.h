#ifndef CASCADECLASSIFIERNODE_H
#define CASCADECLASSIFIERNODE_H

#include <QObject>

#include <fugio/core/uuid.h>
#include <fugio/opencv/uuid.h>
#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/image/image.h>

#include <fugio/nodecontrolbase.h>

#if defined( OPENCV_SUPPORTED )
#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#endif

class CascadeClassifierNode : public fugio::NodeControlBase
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit CascadeClassifierNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~CascadeClassifierNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp );

private:
	static void conversion( CascadeClassifierNode *pNode );

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputFilename;
	QSharedPointer<fugio::PinInterface>			 mPinInputImage;
	QSharedPointer<fugio::PinInterface>			 mPinInputROI;

	QSharedPointer<fugio::PinInterface>			 mPinOutputRects;
	fugio::VariantInterface						*mValOutputRects;

#if defined( OPENCV_SUPPORTED )
	QString										 mFilename;
	cv::CascadeClassifier						 mClassifier;
	cv::Mat										 mMatImg;
	QVector<QRect>								 mRctVec;
	std::vector<cv::Rect>						 mRctTmp;
#endif
};

#endif // CASCADECLASSIFIERNODE_H
