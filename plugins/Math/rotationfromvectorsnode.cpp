#include "rotationfromvectorsnode.h"

#include <fugio/core/uuid.h>
#include <fugio/math/uuid.h>

#include <fugio/math/matrix_interface.h>

#include <QMatrix3x3>
#include <QMatrix4x4>

RotationFromVectorsNode::RotationFromVectorsNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mPinInput1 = pinInput( "Vector3" );
	mPinInput2 = pinInput( "Vector3" );

	mValOutput = pinOutput<fugio::VariantInterface *>( "Matrix", mPinOutput, PID_MATRIX4 );
}

void RotationFromVectorsNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	QVector3D		V1 = variant( mPinInput1 ).value<QVector3D>();
	QVector3D		V2 = variant( mPinInput2 ).value<QVector3D>();

	if( V1.isNull() || V2.isNull() )
	{
		return;
	}

	QMatrix4x4		M4;

	if( V1 != V2 )
	{
		QVector3D	MX = V1.normalized();
		QVector3D	MZ = QVector3D::crossProduct( V1, V2 ).normalized();
		QVector3D	MY = QVector3D::crossProduct( MZ, V1 ).normalized();

		M4.setRow( 0, MX );
		M4.setRow( 1, MY );
		M4.setRow( 2, MZ );
	}

	if( M4 != mValOutput->variant().value<QMatrix4x4>() )
	{
		mValOutput->setVariant( M4 );

		pinUpdated( mPinOutput );
	}
}
