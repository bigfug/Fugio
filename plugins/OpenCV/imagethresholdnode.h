#ifndef IMAGETHRESHOLDNODE_H
#define IMAGETHRESHOLDNODE_H

#include <QObject>

#include <fugio/core/uuid.h>
#include <fugio/opencv/uuid.h>
#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/image/image.h>
#include <fugio/choice_interface.h>

#include <fugio/nodecontrolbase.h>

#if defined( OPENCV_SUPPORTED )
#include <opencv2/core/core.hpp>
#endif

class ImageThresholdNode : public fugio::NodeControlBase
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit ImageThresholdNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~ImageThresholdNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputImage;
	QSharedPointer<fugio::PinInterface>			 mPinInputThreshold;
	QSharedPointer<fugio::PinInterface>			 mPinInputMaxVal;
	QSharedPointer<fugio::PinInterface>			 mPinInputType;

	fugio::ChoiceInterface						*mValInputType;

	QSharedPointer<fugio::PinInterface>			 mPinOutputImage;
	fugio::VariantInterface						*mValOutputImage;

#if defined( OPENCV_SUPPORTED )
	cv::Mat										 mMatImg;
#endif

	static QMap<QString,int>					 mTypeList;
};

#endif // IMAGETHRESHOLDNODE_H
