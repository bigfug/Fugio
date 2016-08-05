#include "regexpnode.h"

#include <QLineEdit>
#include <QDebug>

#include <fugio/pin_control_interface.h>
#include <fugio/context_interface.h>

RegExpNode::RegExpNode( QSharedPointer<fugio::NodeInterface> pNode ) :
	NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_TEXT,		"AF7205D3-204D-4070-AAD0-3EF106935733" );
	FUGID( PIN_INPUT_REGEXP,	"eb2445ea-b47d-4a0d-b4fc-ab1fd0229dda" );
	FUGID( PIN_OUTPUT_TEXT,		"ABDF2527-5590-4FDC-AEC7-E7F93E7E7E89" );

	mPinInput = pinInput( "Input", PIN_INPUT_TEXT);

	mPinRegExp = pinInput( "RegExp", PIN_INPUT_REGEXP );

	mValOutput = pinOutput<fugio::VariantInterface *>( "Output", mPinOutput, PID_STRING_LIST, PIN_OUTPUT_TEXT );
}

void RegExpNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	if( mPinRegExp->isUpdated( pTimeStamp ) || mPinInput->isUpdated( pTimeStamp ) )
	{
		QString			RegTxt = variant( mPinInput ).toString();

		if( RegTxt.isEmpty() )
		{
			return;
		}

		QString			RegStr = variant( mPinRegExp ).toString();

		if( RegStr.isEmpty() )
		{
			return;
		}

		QRegExp			RegExp( RegStr );

		if( !RegExp.isValid() )
		{
			return;
		}

		if( RegExp.captureCount() > 0 )
		{
			QStringList			TxtOut;
			int					RegPos = 0;
			int					MatLen = 0;

			while( ( RegPos = RegExp.indexIn( RegTxt, RegPos ) ) != -1 )
			{
				QStringList		CapTxt = RegExp.capturedTexts();

				if( !CapTxt.isEmpty() )
				{
					CapTxt.removeFirst();

					TxtOut << CapTxt;
				}

				if( ( MatLen = RegExp.matchedLength() ) == 0 )
				{
					break;
				}

				RegPos += MatLen;
			}

			mValOutput->setVariant( TxtOut );

			pinUpdated( mPinOutput );
		}
		else
		{
			mValOutput->setVariant( RegExp.exactMatch( RegTxt ) );

			pinUpdated( mPinOutput );
		}
	}
}
