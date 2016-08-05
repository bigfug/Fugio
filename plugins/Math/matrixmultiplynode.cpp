#include "matrixmultiplynode.h"

#include <QMatrix4x4>
#include <QVector4D>

#include <fugio/core/uuid.h>
#include <fugio/math/uuid.h>
#include <fugio/context_interface.h>

MatrixMultiplyNode::MatrixMultiplyNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mPinMatrix = pinInput( "Matrix" );
	mPinVector = pinInput( "Vector" );

	mPinMatrix->registerPinInputType( PID_MATRIX4 );
	mPinVector->registerPinInputType( PID_VECTOR3 );
	mPinVector->registerPinInputType( PID_VECTOR4 );

	mOutput = pinOutput<fugio::VariantInterface *>( "Matrix", mPinOutput, PID_VECTOR3 );
}

void MatrixMultiplyNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	QMatrix4x4		Mat = variant( mPinMatrix ).value<QMatrix4x4>();
	QVector3D		Vec = variant( mPinVector ).value<QVector3D>();

	QVector4D		Out = Mat * QVector4D( Vec, 1 );

	mOutput->setVariant( QVector3D( Out ) );

	pinUpdated( mPinOutput );
}
