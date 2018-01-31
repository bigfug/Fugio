#include "linebuffernode.h"

#include <fugio/pin_control_interface.h>
#include <fugio/context_interface.h>

#include <QRegExp>

LineBufferNode::LineBufferNode( QSharedPointer<fugio::NodeInterface> pNode ) :
	NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_TEXT,		"AF7205D3-204D-4070-AAD0-3EF106935733" );
	FUGID( PIN_INPUT_SPLIT,		"eb2445ea-b47d-4a0d-b4fc-ab1fd0229dda" );
	FUGID( PIN_INPUT_RESET,		"D677066A-929E-4680-857C-BAC8446278AE" );
	FUGID( PIN_OUTPUT_TEXT,		"ABDF2527-5590-4FDC-AEC7-E7F93E7E7E89" );

	mPinInput = pinInput( "Input", PIN_INPUT_TEXT );

	mPinInputSplit = pinInput( "Split", PIN_INPUT_SPLIT );

	mPinInputReset = pinInput( "Reset", PIN_INPUT_RESET );

	mValOutput = pinOutput<fugio::VariantInterface *>( "Strings", mPinOutput, PID_STRING, PIN_OUTPUT_TEXT );
}

void LineBufferNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	if( mPinInputReset->isUpdated( pTimeStamp ) )
	{
		mBuffer.clear();
	}

	if( mPinInputSplit->isUpdated( pTimeStamp ) || mPinInput->isUpdated( pTimeStamp ) )
	{
		const QRegExp	Split( variant( mPinInputSplit ).toString() );

		if( Split.isEmpty() || !Split.isValid() )
		{
			return;
		}

		QString			Input = variant( mPinInput ).toString();

		if( Input.isEmpty() )
		{
			return;
		}

		if( !mBuffer.isEmpty() )
		{
			Input.prepend( mBuffer );
		}

		QStringList		StrLst;

		int				StrPos = Input.lastIndexOf( Split );

		if( StrPos == -1 )
		{
			mBuffer.append( Input );
		}
		else
		{
			int			EndLen = Input.count() - StrPos;

			mBuffer = Input.right( EndLen );

			Input.remove( StrPos, EndLen );

			StrLst = Input.split( Split, QString::SkipEmptyParts );
		}

		if( mValOutput->variantCount() != StrLst.size() )
		{
			mValOutput->setVariantCount( StrLst.size() );
		}

		for( int i = 0 ; i < StrLst.size() ; i++ )
		{
			mValOutput->setVariant( i, StrLst.at( i ) );
		}

		pinUpdated( mPinOutput );
	}
}
