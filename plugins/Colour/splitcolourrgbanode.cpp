#include "splitcolourrgbanode.h"

#include <fugio/context_interface.h>

SplitColourRGBANode::SplitColourRGBANode( QSharedPointer<fugio::NodeInterface> pNode ) :
	NodeControlBase( pNode )
{
	mPinInput = pinInput( "Colour" );

	mPinInput->registerPinInputType( PID_COLOUR );

	mOutR = pinOutput<fugio::VariantInterface *>( "Red", mPinOutR, PID_FLOAT );

	mOutG = pinOutput<fugio::VariantInterface *>( "Green", mPinOutG, PID_FLOAT );

	mOutB = pinOutput<fugio::VariantInterface *>( "Blue", mPinOutB, PID_FLOAT );

	mOutA = pinOutput<fugio::VariantInterface *>( "Alpha", mPinOutA, PID_FLOAT );
}

void SplitColourRGBANode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	fugio::ColourInterface			*V;
	QColor							 C;

	if( ( V = input<fugio::ColourInterface *>( mPinInput ) ) == 0 )
	{
		return;
	}

	C = V->colour();

	if( mOutR->variant().toFloat() != C.redF() )
	{
		mOutR->setVariant( C.redF() );

		mNode->context()->pinUpdated( mPinOutR );
	}

	if( mOutG->variant().toFloat() != C.greenF() )
	{
		mOutG->setVariant( C.greenF() );

		mNode->context()->pinUpdated( mPinOutG );
	}

	if( mOutB->variant().toFloat() != C.blueF() )
	{
		mOutB->setVariant( C.blueF() );

		mNode->context()->pinUpdated( mPinOutB );
	}

	if( mOutA->variant().toFloat() != C.alphaF() )
	{
		mOutA->setVariant( C.alphaF() );

		mNode->context()->pinUpdated( mPinOutA );
	}
}
