#include "joinvector4node.h"

#include <fugio/core/uuid.h>
#include <fugio/math/uuid.h>

#include <QVector4D>

JoinVector4Node::JoinVector4Node( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mPinX = pinInput( "X" );
	mPinY = pinInput( "Y" );
	mPinZ = pinInput( "Z" );
	mPinW = pinInput( "W" );

	mVector4 = pinOutput<fugio::VariantInterface *>( "Vector4", mPinVector4, PID_VECTOR4 );
}

void JoinVector4Node::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	qreal			NewX = variant( mPinX ).toReal();
	qreal			NewY = variant( mPinY ).toReal();
	qreal			NewZ = variant( mPinZ ).toReal();
	qreal			NewW = variant( mPinW ).toReal();

	QVector4D		CurVec = mVector4->variant().value<QVector4D>();
	QVector4D		NewVec = QVector4D( NewX, NewY, NewZ, NewW );

	if( CurVec != NewVec )
	{
		mVector4->setVariant( NewVec );

		pinUpdated( mPinVector4 );
	}
}
