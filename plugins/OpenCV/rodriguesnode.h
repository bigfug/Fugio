#ifndef RODRIGUESNODE_H
#define RODRIGUESNODE_H

#include <QObject>

#include <fugio/core/uuid.h>
#include <fugio/opencv/uuid.h>
#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>

#include <fugio/nodecontrolbase.h>

#if defined( OPENCV_SUPPORTED )
#include <opencv2/core/core.hpp>
#endif

class RodriguesNode : public fugio::NodeControlBase
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit RodriguesNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~RodriguesNode( void ) Q_DECL_OVERRIDE {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual QUuid uuid( void )
	{
		return( NID_OPENCV_RODRIGUES );
	}

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputMatrix;

	QSharedPointer<fugio::PinInterface>			 mPinOutputMatrix;
	fugio::VariantInterface						*mValOutputMatrix;

	QSharedPointer<fugio::PinInterface>			 mPinOutputJacobian;
	fugio::VariantInterface						*mValOutputJacobian;
};

#endif // RODRIGUESNODE_H
