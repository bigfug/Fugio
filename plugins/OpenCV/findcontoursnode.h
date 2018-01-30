#ifndef FINDCONTOURSNODE_H
#define FINDCONTOURSNODE_H

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

class FindContoursNode : public fugio::NodeControlBase
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit FindContoursNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~FindContoursNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

private:
	static void conversion( FindContoursNode *pNode, qint64 pTimeStamp );

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputImage;

	QSharedPointer<fugio::PinInterface>			 mPinOutputContours;
	fugio::VariantInterface						*mValOutputContours;

	QSharedPointer<fugio::PinInterface>			 mPinOutputHierarchy;
	fugio::VariantInterface						*mValOutputHierarchy;

#if defined( OPENCV_SUPPORTED )
#endif
};

#endif // FINDCONTOURSNODE_H
