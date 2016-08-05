#include "matrixlookatnode.h"

#include <QMatrix4x4>

#include <fugio/core/uuid.h>
#include <fugio/math/uuid.h>

MatrixLookAtNode::MatrixLookAtNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mOutputMatrix = pinOutput<fugio::VariantInterface *>( "Matrix", mPinOutputMatrix, PID_MATRIX4 );

	mPinInputMatrix = pinInput( tr( "Matrix" ) );

	mPinInputMatrix->setValue( QMatrix4x4() );

	mPinInputPosition = pinInput( tr( "Position" ) );

	mPinInputPosition->setValue( QVector3D( 0, 0, 5 ) );

	mPinInputCenter = pinInput( tr( "Center" ) );

	mPinInputCenter->setValue( QVector3D( 0, 0, 0 ) );

	mPinInputUp = pinInput( tr( "Up" ) );

	mPinInputUp->setValue( QVector3D( 0, 1, 0 ) );

	mPinInputMatrix->registerPinInputType( PID_MATRIX4 );
	mPinInputPosition->registerPinInputType( PID_VECTOR3 );
	mPinInputCenter->registerPinInputType( PID_VECTOR3 );
	mPinInputUp->registerPinInputType( PID_VECTOR3 );
}

void MatrixLookAtNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	QVector3D		VecPos;
	QVector3D		VecCenter;
	QVector3D		VecUp;

	VecPos    = variant( mPinInputPosition ).value<QVector3D>();
	VecCenter = variant( mPinInputCenter ).value<QVector3D>();
	VecUp     = variant( mPinInputUp ).value<QVector3D>();

	QMatrix4x4		MatCam;

	MatCam = variant( mPinInputMatrix ).value<QMatrix4x4>();

	MatCam.lookAt( VecPos, VecCenter, VecUp );

	if( MatCam == mOutputMatrix->variant().value<QMatrix4x4>() )
	{
		return;
	}

	mOutputMatrix->setVariant( MatCam );

	pinUpdated( mPinOutputMatrix );
}
