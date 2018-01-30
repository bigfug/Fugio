#include "fontmetricsnode.h"

#include <QFontMetricsF>

#include <fugio/core/uuid.h>

FontMetricsNode::FontMetricsNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_FONT, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_TEXT, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_OUTPUT_BOUNDS, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );

	mPinInputFont = pinInput( "Font", PIN_INPUT_FONT );

	mPinInputText = pinInput( "String", PIN_INPUT_TEXT );

	mValOutputBounds = pinOutput<fugio::VariantInterface *>( "Bounds", mPinOutputBounds, PID_RECT, PIN_OUTPUT_BOUNDS );

	mPinInputFont->setValue( QFont() );
	mPinInputText->setValue( QString() );
}

void FontMetricsNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	QFont			Font   = variant<QFont>( mPinInputFont );
	QString			String = variant<QString>( mPinInputText );

	QFontMetricsF	FM( Font );

	QRectF			Bounds = FM.boundingRect( String );

	if( Bounds != mValOutputBounds->variant().value<QRectF>() )
	{
		mValOutputBounds->setVariant( Bounds );

		pinUpdated( mPinOutputBounds );
	}
}
