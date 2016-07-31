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

bool SplitColourRGBANode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	inputsUpdated( 0 );

	return( true );
}

void SplitColourRGBANode::inputsUpdated( qint64 pTimeStamp )
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
