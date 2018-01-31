#ifndef ABSDIFFNODE_H
#define ABSDIFFNODE_H

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

class AbsDiffNode : public fugio::NodeControlBase
{
	Q_OBJECT

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "AbsDiff" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/AbsDiff_(OpenCV)" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit AbsDiffNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~AbsDiffNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp );

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputImage1;
	QSharedPointer<fugio::PinInterface>			 mPinInputImage2;

	QSharedPointer<fugio::PinInterface>			 mPinOutputImage;
	fugio::VariantInterface						*mValOutputImage;

#if defined( OPENCV_SUPPORTED )
	cv::Mat										 mMatImg;
#endif
};

#endif // ABSDIFFNODE_H
