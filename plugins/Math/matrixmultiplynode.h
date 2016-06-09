#ifndef MATRIXMULTIPLYNODE_H
#define MATRIXMULTIPLYNODE_H

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>

#include <fugio/core/variant_interface.h>

#include <fugio/nodecontrolbase.h>

class MatrixMultiplyNode : public fugio::NodeControlBase
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit MatrixMultiplyNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~MatrixMultiplyNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp );

private:
	QSharedPointer<fugio::PinInterface>			 mPinMatrix;
	QSharedPointer<fugio::PinInterface>			 mPinVector;

	QSharedPointer<fugio::PinInterface>			 mPinOutput;
	fugio::VariantInterface						*mOutput;
};

#endif // MATRIXMULTIPLYNODE_H
