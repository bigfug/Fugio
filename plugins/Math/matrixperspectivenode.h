#ifndef MATRIXPERSPECTIVENODE_H
#define MATRIXPERSPECTIVENODE_H

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>

#include <fugio/core/variant_interface.h>

#include <fugio/nodecontrolbase.h>

class MatrixPerspectiveNode : public fugio::NodeControlBase
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit MatrixPerspectiveNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~MatrixPerspectiveNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp );

private:
	QSharedPointer<fugio::PinInterface>			 mPinAngle;
	QSharedPointer<fugio::PinInterface>			 mPinAspectRatio;
	QSharedPointer<fugio::PinInterface>			 mPinNearPlane;
	QSharedPointer<fugio::PinInterface>			 mPinFarPlane;

	QSharedPointer<fugio::PinInterface>			 mPinOutputMatrix;
	fugio::VariantInterface						*mOutputMatrix;
};

#endif // MATRIXPERSPECTIVENODE_H
