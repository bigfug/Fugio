#include "matrix4node.h"

#include <QSettings>

#include <fugio/math/uuid.h>

Matrix4Node::Matrix4Node( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mPinOutputValue = pinOutput<fugio::MatrixInterface *>( "Matrix", mPinOutput, PID_MATRIX4 );
}

Matrix4Node::~Matrix4Node()
{

}
