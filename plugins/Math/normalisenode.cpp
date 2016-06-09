#include "normalisenode.h"

#include <QVector3D>

#include <fugio/core/uuid.h>
#include <fugio/math/uuid.h>

#include <fugio/context_interface.h>

NormaliseNode::NormaliseNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	static const QUuid	PII_NUMBER1( "{c13a41c6-544b-46bb-a9f2-19dd156d236c}" );
	static const QUuid	PII_NUMBER2( "{608ac771-490b-4ae6-9c81-12b9af526d09}" );

	mPinInput = pinInput( "Vector3", PII_NUMBER1 );

	mValOutput = pinOutput<fugio::VariantInterface *>( "Vector3", mPinOutput, PID_VECTOR3, PII_NUMBER2 );
}

void NormaliseNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	QVector3D		NewVal = variant( mPinInput ).value<QVector3D>().normalized();

	if( NewVal != mValOutput->variant().value<QVector3D>() )
	{
		mValOutput->setVariant( NewVal );

		pinUpdated( mPinOutput );
	}
}
