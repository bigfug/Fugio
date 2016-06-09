#ifndef MATRIXNODE_H
#define MATRIXNODE_H

#include <QObject>

#include <fugio/core/uuid.h>
#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/math/matrix_interface.h>
#include <fugio/nodecontrolbase.h>

class Matrix4Node : public fugio::NodeControlBase
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit Matrix4Node( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~Matrix4Node( void );

	//-------------------------------------------------------------------------
	// NodeControlInterface

private:
	QSharedPointer<fugio::PinInterface>			 mPinOutput;
	fugio::MatrixInterface						*mPinOutputValue;
};

#endif // MATRIXNODE_H
