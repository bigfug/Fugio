#include "splitsizenode.h"

#include <fugio/core/uuid.h>
#include <fugio/context_interface.h>

#include <QSizeF>

SplitSizeNode::SplitSizeNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mPinInput = pinInput( tr( "Size" ) );

	mPinInput->registerPinInputType( PID_SIZE );

	mWidth  = pinOutput<fugio::VariantInterface *>( tr( "Width" ), mPinWidth, PID_FLOAT );

	mHeight = pinOutput<fugio::VariantInterface *>( tr( "Height" ), mPinHeight, PID_FLOAT );
}

void SplitSizeNode::inputsUpdated( qint64 pTimeStamp )
{
	QSizeF				 CurSze = variant( mPinInput ).toSizeF();

	if( !pTimeStamp || mWidth->variant().toReal() != CurSze.width() )
	{
		mWidth->setVariant( CurSze.width() );

		pinUpdated( mPinWidth );
	}

	if( !pTimeStamp || mHeight->variant().toReal() != CurSze.height() )
	{
		mHeight->setVariant( CurSze.height() );

		pinUpdated( mPinHeight );
	}
}
