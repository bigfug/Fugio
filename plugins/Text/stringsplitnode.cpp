#include "stringsplitnode.h"

StringSplitNode::StringSplitNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_STRING, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_SPLIT, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_OUTPUT_LIST, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );

	mPinInputString = pinInput( "String", PIN_INPUT_STRING );

	mPinInputSplit = pinInput( "Split", PIN_INPUT_SPLIT );

	mValOutputList = pinOutput<fugio::VariantInterface *>( "Strings", mPinOutputList, PID_STRING, PIN_OUTPUT_LIST );
}

void StringSplitNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	QString		String = variant( mPinInputString ).toString();
	QString		Split  = variant( mPinInputSplit ).toString();
	QStringList	List = String.split( Split );

	if( mValOutputList->variantCount() != List.size() )
	{
		mValOutputList->setVariantCount( List.size() );
	}

	for( int i = 0 ; i < List.size() ; i++ )
	{
		mValOutputList->setVariant( i, List.at( i ) );
	}

	pinUpdated( mPinOutputList );
}
