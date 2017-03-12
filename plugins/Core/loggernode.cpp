#include "loggernode.h"


LoggerNode::LoggerNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_STRING, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );

	mPinInputString = pinInput( "String", PIN_INPUT_STRING );
}

void LoggerNode::inputsUpdated( qint64 pTimeStamp )
{
	if( mPinInputString->isUpdated( pTimeStamp ) )
	{
		QString		S = variant( mPinInputString ).toString();

		if( !S.isEmpty() )
		{
			qInfo() << S;
		}
	}
}
