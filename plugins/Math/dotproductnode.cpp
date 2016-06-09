#include "dotproductnode.h"

#include <QVector3D>

#include <fugio/core/uuid.h>
#include <fugio/math/uuid.h>

#include <fugio/context_interface.h>

DotProductNode::DotProductNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	static const QUuid	PII_NUMBER1( "{c13a41c6-544b-46bb-a9f2-19dd156d236c}" );
	static const QUuid	PII_NUMBER2( "{a145ebb2-9166-4a29-bccf-8ee184e27406}" );

	mPinInput1 = pinInput( "Vector3", PII_NUMBER1 );
	mPinInput2 = pinInput( "Vector3", PII_NUMBER2 );

	mPinInput1->registerPinInputType( PID_VECTOR3 );
	mPinInput2->registerPinInputType( PID_VECTOR3 );

	mValOutput = pinOutput<fugio::VariantInterface *>( "Number", mPinOutput, PID_FLOAT );
}

void DotProductNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	QVector3D	v1 = variant( mPinInput1 ).value<QVector3D>();
	QVector3D	v2 = variant( mPinInput2 ).value<QVector3D>();

	qreal		d = QVector3D::dotProduct( v1, v2 );

	if( d != mValOutput->variant().toReal() )
	{
		mValOutput->setVariant( d );

		pinUpdated( mPinOutput );
	}
}
