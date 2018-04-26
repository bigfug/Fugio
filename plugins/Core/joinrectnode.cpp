#include "joinrectnode.h"

#include <QRectF>

#include <fugio/core/uuid.h>

#include <fugio/pin_variant_iterator.h>

JoinRectNode::JoinRectNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_X,			"B8397BEA-CCB5-459B-B952-0C14B1527E0C" );
	FUGID( PIN_INPUT_Y,			"78EE5967-367D-4B6A-9876-27EBD1539800" );
	FUGID( PIN_INPUT_WIDTH,		"0CF61055-152D-47C1-803C-43A88BC38C2F" );
	FUGID( PIN_INPUT_HEIGHT,	"4A9149DB-6E1E-4680-8E81-E8478FB7E3FA" );
	FUGID( PIN_OUTPUT_RECT,		"87DF03AA-D3DE-4F44-8201-AB5580585481" );

	mPinInputX = pinInput( "X", PIN_INPUT_X );
	mPinInputY = pinInput( "Y", PIN_INPUT_Y );
	mPinInputWidth = pinInput( "Width", PIN_INPUT_WIDTH );
	mPinInputHeight = pinInput( "Height", PIN_INPUT_HEIGHT );

	mValOutputRect = pinOutput<fugio::VariantInterface *>( "Rect", mPinOutputRect, PID_RECT, PIN_OUTPUT_RECT );
}

void JoinRectNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	bool	UpdateOutput = false;

	fugio::PinVariantIterator	X( mPinInputX );
	fugio::PinVariantIterator	Y( mPinInputY );
	fugio::PinVariantIterator	W( mPinInputWidth );
	fugio::PinVariantIterator	H( mPinInputHeight );

	QVector<int>	PinCnt = { X.count(), Y.count(), W.count(), H.count() };

	auto			MinMax = std::minmax_element( PinCnt.begin(), PinCnt.end() );

	if( !*MinMax.first )
	{
		return;
	}

	const int		OutCnt = *MinMax.second;

	variantSetCount( mValOutputRect, OutCnt, UpdateOutput );

	for( int i = 0 ; i < OutCnt ; i++ )
	{
		qreal		x = X.index( i ).toReal();
		qreal		y = Y.index( i ).toReal();
		qreal		w = W.index( i ).toReal();
		qreal		h = H.index( i ).toReal();

		variantSetValue( mValOutputRect, i, QRectF( x, y, w, h ), UpdateOutput );
	}

	if( UpdateOutput )
	{
		pinUpdated( mPinOutputRect );
	}
}
