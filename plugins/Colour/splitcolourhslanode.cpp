#include "splitcolourhslanode.h"

#include <fugio/context_interface.h>

SplitColourHSLANode::SplitColourHSLANode( QSharedPointer<fugio::NodeInterface> pNode ) :
	NodeControlBase( pNode )
{
	mPinInput = pinInput( "Colour" );

	mPinInput->registerPinInputType( PID_COLOUR );

	mOutH = pinOutput<fugio::VariantInterface *>( "Hue", mPinOutH, PID_FLOAT );

	mOutS = pinOutput<fugio::VariantInterface *>( "Saturation", mPinOutS, PID_FLOAT );

	mOutL = pinOutput<fugio::VariantInterface *>( "Lightness", mPinOutL, PID_FLOAT );

	mOutA = pinOutput<fugio::VariantInterface *>( "Alpha", mPinOutA, PID_FLOAT );
}

void SplitColourHSLANode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	QColor							 C;

	fugio::ColourInterface			*ColInt = input<fugio::ColourInterface *>( mPinInput );

	if( ColInt )
	{
		C = ColInt->colour();
	}
	else
	{
		C = variant( mPinInput ).value<QColor>();
	}

	if( mOutH->variant().toFloat() != C.hslHueF() )
	{
		mOutH->setVariant( C.hslHueF() );

		mNode->context()->pinUpdated( mPinOutH );
	}

	if( mOutS->variant().toFloat() != C.hslSaturationF() )
	{
		mOutS->setVariant( C.hslSaturationF() );

		mNode->context()->pinUpdated( mPinOutS );
	}

	if( mOutL->variant().toFloat() != C.lightnessF() )
	{
		mOutL->setVariant( C.lightnessF() );

		mNode->context()->pinUpdated( mPinOutL );
	}

	if( mOutA->variant().toFloat() != C.alphaF() )
	{
		mOutA->setVariant( C.alphaF() );

		mNode->context()->pinUpdated( mPinOutA );
	}
}
