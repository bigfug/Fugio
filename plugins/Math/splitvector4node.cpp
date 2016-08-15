#include "splitvector4node.h"

#include <QVector4D>

#include <fugio/context_interface.h>
#include <fugio/math/uuid.h>

SplitVector4Node::SplitVector4Node( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mPinInput = pinInput( "Vector4" );

	mPinInput->registerPinInputType( PID_VECTOR4 );

	mOutX = pinOutput<fugio::VariantInterface *>( "X", mPinOutX, PID_FLOAT );

	mOutY = pinOutput<fugio::VariantInterface *>( "Y", mPinOutY, PID_FLOAT );

	mOutZ = pinOutput<fugio::VariantInterface *>( "Z", mPinOutZ, PID_FLOAT );

	mOutW = pinOutput<fugio::VariantInterface *>( "W", mPinOutW, PID_FLOAT );
}

void SplitVector4Node::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	QVector4D				 V4 = variant( mPinInput ).value<QVector4D>();

	if( mOutX->variant().toFloat() != V4.x() )
	{
		mOutX->setVariant( V4.x() );

		pinUpdated( mPinOutX );
	}

	if( mOutY->variant().toFloat() != V4.y() )
	{
		mOutY->setVariant( V4.y() );

		pinUpdated( mPinOutY );
	}

	if( mOutZ->variant().toFloat() != V4.z() )
	{
		mOutZ->setVariant( V4.z() );

		pinUpdated( mPinOutZ );
	}

	if( mOutW->variant().toFloat() != V4.w() )
	{
		mOutW->setVariant( V4.w() );

		pinUpdated( mPinOutW );
	}
}
