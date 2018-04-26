#include "stringtofloatnode.h"

#include <fugio/core/uuid.h>

StringToFloatNode::StringToFloatNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_STRING, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_OUTPUT_FLOAT, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );

	mPinInputString = pinInput( "String", PIN_INPUT_STRING );

	mValOutputValue = pinOutput<fugio::VariantInterface *>( "Number", mPinOutputValue, PID_FLOAT, PIN_OUTPUT_FLOAT );
}

void StringToFloatNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	QString		S = variant( mPinInputString ).toString();
	bool		C;
	double		V = S.toDouble( &C );

	if( C && ( mPinOutputValue->alwaysUpdate() || V != mValOutputValue->variant().toDouble() ) )
	{
		mValOutputValue->setVariant( V );

		pinUpdated( mPinOutputValue );
	}
}
