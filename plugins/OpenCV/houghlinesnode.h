#ifndef HOUGHLINESNODE_H
#define HOUGHLINESNODE_H

#define _USE_MATH_DEFINES

#include <cmath>

#include <QObject>
#include <QLineF>

#include <fugio/image/image.h>
#include <fugio/core/array_interface.h>

#include <fugio/nodecontrolbase.h>

#if defined( OPENCV_SUPPORTED )
#include <opencv2/core/core.hpp>
#endif

class HoughLinesNode : public fugio::NodeControlBase
{
	Q_OBJECT

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Detect lines in an image" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Hough_Lines_(OpenCV)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit HoughLinesNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~HoughLinesNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp );

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputImage;
	QSharedPointer<fugio::PinInterface>			 mPinInputRho;
	QSharedPointer<fugio::PinInterface>			 mPinInputTheta;
	QSharedPointer<fugio::PinInterface>			 mPinInputThreshold;
	QSharedPointer<fugio::PinInterface>			 mPinInputMinLineLength;
	QSharedPointer<fugio::PinInterface>			 mPinInputMaxLineGap;

	QSharedPointer<fugio::PinInterface>			 mPinOutputLines;
	fugio::VariantInterface						*mValOutputLines;

//	QVector<QLineF>								 mLineData;

#if defined( OPENCV_SUPPORTED )
	cv::Mat										 mMatImg;
#endif
};

#endif // HOUGHLINESNODE_H
