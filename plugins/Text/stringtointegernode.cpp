#include "stringtointegernode.h"

#include <fugio/core/uuid.h>

StringToIntegerNode::StringToIntegerNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_STRING, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_OUTPUT_VALUE, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );

	mPinInputString = pinInput( "String", PIN_INPUT_STRING );

	mValOutputValue = pinOutput<fugio::VariantInterface *>( "Number", mPinOutputValue, PID_INTEGER, PIN_OUTPUT_VALUE );
}

void StringToIntegerNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	QString		S = variant( mPinInputString ).toString();
	bool		C;
	int			V = S.toInt( &C );

	if( C && V != mValOutputValue->variant().toInt() )
	{
		mValOutputValue->setVariant( V );

		pinUpdated( mPinOutputValue );
	}
}
