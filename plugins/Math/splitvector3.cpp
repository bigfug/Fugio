#include "splitvector3.h"

#include <QVector3D>

#include <fugio/context_interface.h>
#include <fugio/math/uuid.h>

SplitVector3Node::SplitVector3Node( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mPinInput = pinInput( "Vector3" );

	mPinInput->registerPinInputType( PID_VECTOR3 );

	mOutX = pinOutput<fugio::VariantInterface *>( "X", mPinOutX, PID_FLOAT );

	mOutY = pinOutput<fugio::VariantInterface *>( "Y", mPinOutY, PID_FLOAT );

	mOutZ = pinOutput<fugio::VariantInterface *>( "Z", mPinOutZ, PID_FLOAT );
}

void SplitVector3Node::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	QVector3D				 V3 = variant( mPinInput ).value<QVector3D>();

	if( mOutX->variant().toFloat() != V3.x() )
	{
		mOutX->setVariant( V3.x() );

		mNode->context()->pinUpdated( mPinOutX );
	}

	if( mOutY->variant().toFloat() != V3.y() )
	{
		mOutY->setVariant( V3.y() );

		mNode->context()->pinUpdated( mPinOutY );
	}

	if( mOutZ->variant().toFloat() != V3.z() )
	{
		mOutZ->setVariant( V3.z() );

		mNode->context()->pinUpdated( mPinOutZ );
	}
}
