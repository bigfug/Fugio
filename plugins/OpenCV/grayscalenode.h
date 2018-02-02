#ifndef GRAYSCALENODE_H
#define GRAYSCALENODE_H

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

class GrayscaleNode : public fugio::NodeControlBase
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit GrayscaleNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~GrayscaleNode( void );

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual QUuid uuid( void )
	{
		return( NID_OPENCV_GRAYSCALE );
	}

	virtual void inputsUpdated( qint64 pTimeStamp );

private:
	static void conversion( GrayscaleNode *pNode );

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputImage;

	QSharedPointer<fugio::PinInterface>			 mPinOutputImage;
	fugio::VariantInterface								*mValOutputImage;

#if defined( OPENCV_SUPPORTED )
	cv::Mat									 mMatImg;
#endif
};

#endif // GRAYSCALENODE_H
