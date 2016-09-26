#include "matrixorthographicnode.h"

#include <QMatrix4x4>

#include <fugio/core/uuid.h>
#include <fugio/math/uuid.h>
#include <fugio/context_interface.h>

MatrixOrthographicNode::MatrixOrthographicNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mOutputMatrix = pinOutput<fugio::VariantInterface *>( "Matrix", mPinOutputMatrix, PID_MATRIX4 );

	mPinInputLeft   = pinInput( tr( "Left" ) );
	mPinInputRight  = pinInput( tr( "Right" ) );
	mPinInputBottom = pinInput( tr( "Bottom" ) );
	mPinInputTop    = pinInput( tr( "Top" ) );
	mPinInputNear   = pinInput( tr( "Near" ) );
	mPinInputFar    = pinInput( tr( "Far" ) );

	mPinInputLeft->registerPinInputType( PID_FLOAT );
	mPinInputRight->registerPinInputType( PID_FLOAT );
	mPinInputBottom->registerPinInputType( PID_FLOAT );
	mPinInputTop->registerPinInputType( PID_FLOAT );
	mPinInputNear->registerPinInputType( PID_FLOAT );
	mPinInputFar->registerPinInputType( PID_FLOAT );

	mPinInputLeft->setValue( -10.0 );
	mPinInputRight->setValue( 10.0 );
	mPinInputBottom->setValue( -10.0 );
	mPinInputTop->setValue( 10.0 );
	mPinInputNear->setValue( 0.1 );
	mPinInputFar->setValue( 100.0 );
}

void MatrixOrthographicNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	QMatrix4x4		MatPrj;

	qreal			Left   = variant( mPinInputLeft ).toReal();
	qreal			Right  = variant( mPinInputRight ).toReal();
	qreal			Bottom = variant( mPinInputBottom ).toReal();
	qreal			Top    = variant( mPinInputTop ).toReal();
	qreal			Near   = variant( mPinInputNear ).toReal();
	qreal			Far    = variant( mPinInputFar ).toReal();

	MatPrj.ortho( Left, Right, Bottom, Top, Near, Far );

	mOutputMatrix->setVariant( MatPrj );

	pinUpdated( mPinOutputMatrix );
}
