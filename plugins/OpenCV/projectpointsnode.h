#ifndef PROJECTPOINTSNODE_H
#define PROJECTPOINTSNODE_H

#include <QObject>

#include <fugio/core/uuid.h>
#include <fugio/opencv/uuid.h>
#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>

#include <fugio/nodecontrolbase.h>

#if defined( OPENCV_SUPPORTED )
#include <opencv2/core/core.hpp>
#endif

class ProjectPointsNode : public fugio::NodeControlBase
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit ProjectPointsNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~ProjectPointsNode( void ) Q_DECL_OVERRIDE {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual QUuid uuid( void )
	{
		return( NID_OPENCV_PROJECT_POINTS );
	}

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputObjectPoints;
	QSharedPointer<fugio::PinInterface>			 mPinInputRotation;
	QSharedPointer<fugio::PinInterface>			 mPinInputTranslation;
	QSharedPointer<fugio::PinInterface>			 mPinInputCameraMatrix;
	QSharedPointer<fugio::PinInterface>			 mPinInputDistCoeffs;

	QSharedPointer<fugio::PinInterface>			 mPinOutputImagePoints;
	fugio::VariantInterface						*mValOutputImagePoints;
};

#endif // PROJECTPOINTSNODE_H
