#ifndef MOMENTSNODE_H
#define MOMENTSNODE_H

#include <QObject>

#include <fugio/core/uuid.h>
#include <fugio/opencv/uuid.h>
#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/image/image.h>
#include <fugio/core/variant_interface.h>

#include <fugio/nodecontrolbase.h>

#if defined( OPENCV_SUPPORTED )
#include <opencv2/core/core.hpp>
#endif

class MomentsNode : public fugio::NodeControlBase
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit MomentsNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~MomentsNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp );

private:

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputImage;

	QSharedPointer<fugio::PinInterface>			 mPinOutputArea;
	QSharedPointer<fugio::PinInterface>			 mPinOutputPosition;

	fugio::VariantInterface						*mValOutputArea;
	fugio::VariantInterface						*mValOutputPosition;
};

#endif // MOMENTSNODE_H
