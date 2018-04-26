#ifndef RESIZENODE_H
#define RESIZENODE_H

#include <fugio/core/uuid.h>
#include <fugio/opencv/uuid.h>
#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/image/image.h>

#include <fugio/nodecontrolbase.h>

#include <fugio/choice_interface.h>

#if defined( OPENCV_SUPPORTED )
#include <opencv2/core/core.hpp>
#endif

class ResizeNode : public fugio::NodeControlBase
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit ResizeNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~ResizeNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

private:
	//static void conversion( ScaleImageNode *pNode );

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputImage;
	QSharedPointer<fugio::PinInterface>			 mPinInputSize;
	QSharedPointer<fugio::PinInterface>			 mPinInputScaleX;
	QSharedPointer<fugio::PinInterface>			 mPinInputScaleY;

	QSharedPointer<fugio::PinInterface>			 mPinInputInterpolation;
	fugio::ChoiceInterface						*mValInputInterpolation;

	QSharedPointer<fugio::PinInterface>			 mPinOutputImage;
	fugio::VariantInterface						*mValOutputImage;

#if defined( OPENCV_SUPPORTED )
	static QMap<int,QString>					 mInterpolationMap;

	cv::Mat										 mMatImg;
#endif
};

#endif // RESIZENODE_H
