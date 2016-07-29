#include "joinvector3.h"

#include <fugio/core/uuid.h>
#include <fugio/math/uuid.h>

#include <QVector3D>

JoinVector3Node::JoinVector3Node( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mPinX = pinInput( "X" );
	mPinY = pinInput( "Y" );
	mPinZ = pinInput( "Z" );

	mVector3 = pinOutput<fugio::VariantInterface *>( "Vector3", mPinVector3, PID_VECTOR3 );
}

void JoinVector3Node::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	qreal			NewX = variant( mPinX ).toReal();
	qreal			NewY = variant( mPinY ).toReal();
	qreal			NewZ = variant( mPinZ ).toReal();

	QVector3D		CurVec = mVector3->variant().value<QVector3D>();
	QVector3D		NewVec = QVector3D( NewX, NewY, NewZ );

	if( CurVec != NewVec )
	{
		mVector3->setVariant( NewVec );

		pinUpdated( mPinVector3 );
	}
}
