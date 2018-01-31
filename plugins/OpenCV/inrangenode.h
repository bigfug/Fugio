#ifndef INRANGENODE_H
#define INRANGENODE_H

#include <fugio/core/uuid.h>
#include <fugio/opencv/uuid.h>
#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/image/image.h>

#include <fugio/nodecontrolbase.h>

#if defined( OPENCV_SUPPORTED )
#include <opencv2/core/core.hpp>
#endif

class InRangeNode : public fugio::NodeControlBase
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit InRangeNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~InRangeNode( void );

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

private:
	static void conversion( InRangeNode *pNode );

	static void pin2ints( InRangeNode *pNode, QSharedPointer<fugio::PinInterface> &P, int &i1, int &i2, int &i3 );

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputImage;
	QSharedPointer<fugio::PinInterface>			 mPinInputLow;
	QSharedPointer<fugio::PinInterface>			 mPinInputHigh;

	QSharedPointer<fugio::PinInterface>			 mPinOutputImage;
	fugio::VariantInterface								*mValOutputImage;

#if defined( OPENCV_SUPPORTED )
	cv::Mat									 mMatImg;
#endif
};


#endif // INRANGENODE_H
