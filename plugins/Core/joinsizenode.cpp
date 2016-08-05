#include "joinsizenode.h"

#include <fugio/core/uuid.h>

#include <QSizeF>

JoinSizeNode::JoinSizeNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mPinWidth  = pinInput( tr( "Width" ) );
	mPinHeight = pinInput( tr( "Height" ) );

	mSize = pinOutput<fugio::VariantInterface *>( tr( "Size" ), mPinSize, PID_SIZE );
}

void JoinSizeNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	qreal			NewW = variant( mPinWidth ).toReal();
	qreal			NewH = variant( mPinHeight ).toReal();

	QSizeF			CurSze = mSize->variant().toSizeF();
	QSizeF			NewSze = QSizeF( NewW, NewH );

	if( CurSze != NewSze )
	{
		mSize->setVariant( NewSze );

		pinUpdated( mPinSize );
	}
}
