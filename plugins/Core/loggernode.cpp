#include "loggernode.h"

#include <fugio/core/list_interface.h>

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
		fugio::ListInterface	*L = input<fugio::ListInterface *>( mPinInputString );

		if( L )
		{
			for( int i = 0 ; i < L->listSize() ; i++ )
			{
				QString		S = L->listIndex( i ).toString();

				if( !S.isEmpty() )
				{
					qInfo() << S;
				}
			}
		}
		else
		{
			QString		S = variant( mPinInputString ).toString();

			if( !S.isEmpty() )
			{
				qInfo() << S;
			}
		}
	}
}
