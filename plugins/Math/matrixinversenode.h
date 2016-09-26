#ifndef MATRIXINVERSENODE_H
#define MATRIXINVERSENODE_H

#include <fugio/nodecontrolbase.h>

class MatrixInverseNode : public fugio::NodeControlBase
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit MatrixInverseNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~MatrixInverseNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp );

private:
	QSharedPointer<fugio::PinInterface>			 mPinMatrixInput;

	QSharedPointer<fugio::PinInterface>			 mPinMatrixOutput;
	fugio::VariantInterface						*mValMatrixOutput;
};

#endif // MATRIXINVERSENODE_H
