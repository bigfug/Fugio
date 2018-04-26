#include "joinsizenode.h"

#include <fugio/core/uuid.h>

#include <QSizeF>

#include <fugio/pin_variant_iterator.h>

JoinSizeNode::JoinSizeNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_WIDTH, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_HEIGHT, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_OUTPUT_SIZE, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );

	mPinInputWidth  = pinInput( tr( "Width" ), PIN_INPUT_WIDTH );
	mPinInputHeight = pinInput( tr( "Height" ), PIN_INPUT_HEIGHT );

	mValOutputSize = pinOutput<fugio::VariantInterface *>( tr( "Size" ), mPinOutputSize, PID_SIZE, PIN_OUTPUT_SIZE );
}

void JoinSizeNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	bool	UpdateOutput = false;

	fugio::PinVariantIterator	W( mPinInputWidth );
	fugio::PinVariantIterator	H( mPinInputHeight );

	QVector<int>	PinCnt = {  W.count(), H.count() };

	auto			MinMax = std::minmax_element( PinCnt.begin(), PinCnt.end() );

	if( !*MinMax.first )
	{
		return;
	}

	const int		OutCnt = *MinMax.second;

	variantSetCount( mValOutputSize, OutCnt, UpdateOutput );

	for( int i = 0 ; i < OutCnt ; i++ )
	{
		qreal		w = W.index( i ).toReal();
		qreal		h = H.index( i ).toReal();

		variantSetValue( mValOutputSize, i, QSizeF( w, h ), UpdateOutput );
	}

	if( UpdateOutput )
	{
		pinUpdated( mPinOutputSize );
	}
}
