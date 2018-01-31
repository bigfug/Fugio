#ifndef INPAINTNODE_H
#define INPAINTNODE_H

#include <QObject>

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

class InPaintNode : public fugio::NodeControlBase
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit InPaintNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~InPaintNode( void );

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual QUuid uuid( void )
	{
		return( NID_OPENCV_INPAINT );
	}

	virtual void inputsUpdated( qint64 pTimeStamp );

	virtual bool initialise( void );

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputImage;
	QSharedPointer<fugio::PinInterface>			 mPinInputMask;
	QSharedPointer<fugio::PinInterface>			 mPinInputRadius;

	QSharedPointer<fugio::PinInterface>			 mPinOutputImage;
	fugio::VariantInterface							*mValOutputImage;

#if defined( OPENCV_SUPPORTED )
	cv::Mat									 mMatImg;
#endif
};

#endif // INPAINTNODE_H
