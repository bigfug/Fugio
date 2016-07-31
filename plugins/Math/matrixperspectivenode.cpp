#include "matrixperspectivenode.h"

#include <QMatrix4x4>

#include <fugio/core/uuid.h>
#include <fugio/math/uuid.h>
#include <fugio/context_interface.h>

MatrixPerspectiveNode::MatrixPerspectiveNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mOutputMatrix = pinOutput<fugio::VariantInterface *>( "Matrix", mPinOutputMatrix, PID_MATRIX4 );

	mPinAngle       = pinInput( tr( "Vertical Angle" ) );
	mPinAspectRatio = pinInput( tr( "Aspect Ratio" ) );
	mPinNearPlane   = pinInput( tr( "Near Plane" ) );
	mPinFarPlane    = pinInput( tr( "Far Plane" ) );

	mPinAngle->registerPinInputType( PID_FLOAT );

	mPinAngle->setValue( 45.0 );
	mPinAspectRatio->setValue( 4.0 / 3.0 );
	mPinNearPlane->setValue( 0.1 );
	mPinFarPlane->setValue( 100.0 );
}

void MatrixPerspectiveNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	QMatrix4x4		MatPrj;

	qreal			Angle       = variant( mPinAngle ).toReal();
	qreal			AspectRatio = variant( mPinAspectRatio ).toReal();
	qreal			NearPlane   = variant( mPinNearPlane ).toReal();
	qreal			FarPlane    = variant( mPinFarPlane ).toReal();

	MatPrj.perspective( Angle, AspectRatio, NearPlane, FarPlane );

	mOutputMatrix->setVariant( MatPrj );

	pinUpdated( mPinOutputMatrix );
}
