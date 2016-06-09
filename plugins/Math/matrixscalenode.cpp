#include "matrixscalenode.h"

#include <fugio/context_interface.h>
#include <fugio/math/uuid.h>

#include <QMatrix4x4>

MatrixScaleNode::MatrixScaleNode( QSharedPointer<fugio::NodeInterface> pNode ) :
	NodeControlBase( pNode ), mX( -1 ), mY( -1 ), mZ( -1 )
{
	mValue = pinOutput<fugio::VariantInterface *>( "Matrix", mPinValue, PID_MATRIX4 );

	mPinInputMatrix = pinInput( "Matrix" );

	mPinInputX = pinInput( "X" );
	mPinInputY = pinInput( "Y" );
	mPinInputZ = pinInput( "Z" );

	mPinInputX->setValue( 1.0 );
	mPinInputY->setValue( 1.0 );
	mPinInputZ->setValue( 1.0 );
}

void MatrixScaleNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	qreal				 x = variant( mPinInputX ).toReal();
	qreal				 y = variant( mPinInputY ).toReal();
	qreal				 z = variant( mPinInputZ ).toReal();

	QMatrix4x4			 m = variant( mPinInputMatrix ).value<QMatrix4x4>();

	m.scale( x, y, z );

	//if( MatRot != mM )
	{
		mValue->setVariant( m );
	}

	mX = x; mY = y; mZ = z;
}
