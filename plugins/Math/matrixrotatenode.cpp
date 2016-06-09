#include "matrixrotatenode.h"

#include <fugio/context_interface.h>
#include <fugio/math/uuid.h>

#include <QMatrix4x4>

MatrixRotateNode::MatrixRotateNode( QSharedPointer<fugio::NodeInterface> pNode ) :
	NodeControlBase( pNode )
{
	mValue = pinOutput<fugio::VariantInterface *>( "Matrix", mPinValue, PID_MATRIX4 );

	mPinInputMatrix = pinInput( "Matrix Input" );

	mPinInputX = pinInput( "X" );
	mPinInputY = pinInput( "Y" );
	mPinInputZ = pinInput( "Z" );
}

MatrixRotateNode::~MatrixRotateNode( void )
{

}

bool MatrixRotateNode::initialise()
{
	mPinInputX->setValue( 0.0 );
	mPinInputY->setValue( 0.0 );
	mPinInputZ->setValue( 0.0 );

	return( true );
}

void MatrixRotateNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	qreal				 x = variant( mPinInputX ).toReal();
	qreal				 y = variant( mPinInputY ).toReal();
	qreal				 z = variant( mPinInputZ ).toReal();

	QMatrix4x4		MatRot = variant( mPinInputMatrix ).value<QMatrix4x4>();

	MatRot.rotate( x, 1, 0, 0 );
	MatRot.rotate( y, 0, 1, 0 );
	MatRot.rotate( z, 0, 0, 1 );

	QVariant		MatVar = MatRot;

	mValue->setVariant( MatVar );

	pinUpdated( mPinValue );
}
