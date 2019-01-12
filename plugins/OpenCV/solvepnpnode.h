#ifndef SOLVEPNPNODE_H
#define SOLVEPNPNODE_H

#include <QObject>

#include <fugio/core/uuid.h>
#include <fugio/opencv/uuid.h>
#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>

#include <fugio/nodecontrolbase.h>

#if defined( OPENCV_SUPPORTED )
#include <opencv2/core/core.hpp>
#endif

class SolvePNPNode : public fugio::NodeControlBase
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit SolvePNPNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~SolvePNPNode( void ) Q_DECL_OVERRIDE {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual QUuid uuid( void )
	{
		return( NID_OPENCV_SOLVEPNP );
	}

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputObjectPoints;
	QSharedPointer<fugio::PinInterface>			 mPinInputImagePoints;
	QSharedPointer<fugio::PinInterface>			 mPinInputCameraArray;
	QSharedPointer<fugio::PinInterface>			 mPinInputDistCoeffs;

	QSharedPointer<fugio::PinInterface>			 mPinOutputRotation;
	fugio::VariantInterface						*mValOutputRotation;

	QSharedPointer<fugio::PinInterface>			 mPinOutputTranslation;
	fugio::VariantInterface						*mValOutputTranslation;
};

#endif // SOLVEPNPNODE_H
