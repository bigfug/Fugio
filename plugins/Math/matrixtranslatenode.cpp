#include "matrixtranslatenode.h"

#include <fugio/context_interface.h>
#include <fugio/math/uuid.h>

#include <QMatrix4x4>

MatrixTranslateNode::MatrixTranslateNode( QSharedPointer<fugio::NodeInterface> pNode ) :
	NodeControlBase( pNode )
{
	mValue = pinOutput<fugio::VariantInterface *>( "Matrix", mPinValue, PID_MATRIX4 );

	mPinInputMatrix = pinInput( "Matrix" );

	mPinInputX = pinInput( "X" );
	mPinInputY = pinInput( "Y" );
	mPinInputZ = pinInput( "Z" );

	mPinInputX->setValue( 0.0 );
	mPinInputY->setValue( 0.0 );
	mPinInputZ->setValue( 0.0 );
}

void MatrixTranslateNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	qreal				 x = variant( mPinInputX ).toReal();
	qreal				 y = variant( mPinInputY ).toReal();
	qreal				 z = variant( mPinInputZ ).toReal();

	QMatrix4x4			 m = variant( mPinInputMatrix ).value<QMatrix4x4>();

	m.translate( x, y, z );

	if( m != mValue->variant().value<QMatrix4x4>() )
	{
		mValue->setVariant( m );

		pinUpdated( mPinValue );
	}
}
