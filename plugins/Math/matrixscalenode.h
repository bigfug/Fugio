#ifndef MATRIXSCALENODE_H
#define MATRIXSCALENODE_H

#include <QObject>
#include <QSharedPointer>
#include <QMatrix4x4>

#include <fugio/node_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/core/variant_interface.h>

#include <fugio/core/uuid.h>

#include <fugio/nodecontrolbase.h>

class MatrixScaleNode : public fugio::NodeControlBase
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit MatrixScaleNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~MatrixScaleNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp );

private:
	QSharedPointer<fugio::PinInterface>			 mPinValue;
	fugio::VariantInterface						*mValue;

	QSharedPointer<fugio::PinInterface>			 mPinInputMatrix;
	QSharedPointer<fugio::PinInterface>			 mPinInputX;
	QSharedPointer<fugio::PinInterface>			 mPinInputY;
	QSharedPointer<fugio::PinInterface>			 mPinInputZ;

	qreal									 mX, mY, mZ;

	QMatrix4x4								 mM;
};

#endif // MATRIXSCALENODE_H
