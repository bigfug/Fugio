#include "splitsizenode.h"

#include <fugio/core/uuid.h>
#include <fugio/context_interface.h>

#include <QSizeF>
#include <QRectF>

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
	if( mPinInput->isUpdated( pTimeStamp ) )
	{
		fugio::VariantInterface		*V = input<fugio::VariantInterface *>( mPinInput );

		QSizeF						 S;

		if( V )
		{
			switch( QMetaType::Type( V->variant().type() ) )
			{
				case QMetaType::QSize:
					S = V->variant().toSize();
					break;

				case QMetaType::QSizeF:
					S = V->variant().toSizeF();
					break;

				case QMetaType::QRect:
					S = V->variant().toRect().size();
					break;

				case QMetaType::QRectF:
					S = V->variant().toRectF().size();
					break;
			}
		}

		if( mWidth->variant().toReal() != S.width() )
		{
			mWidth->setVariant( S.width() );

			pinUpdated( mPinWidth );
		}

		if( mHeight->variant().toReal() != S.height() )
		{
			mHeight->setVariant( S.height() );

			pinUpdated( mPinHeight );
		}
	}
}
