#include "stringjoinnode.h"

#include <fugio/context_interface.h>

#include <fugio/pin_variant_iterator.h>

StringJoinNode::StringJoinNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_SEPERATOR, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_OUTPUT_STRING, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_INPUT_STRING1, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );
	FUGID( PIN_INPUT_STRING2, "249f2932-f483-422f-b811-ab679f006381" );

	mPinInputJoinChar = pinInput( "Seperator", PIN_INPUT_SEPERATOR );

	mValOutput = pinOutput<fugio::VariantInterface *>( "String", mPinOutput, PID_STRING, PIN_OUTPUT_STRING );

	pinInput( "Input", PIN_INPUT_STRING1 );
	pinInput( "Input", PIN_INPUT_STRING2 );
}

void StringJoinNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	if( !pTimeStamp )
	{
		return;
	}

	QStringList		StrLst;
	QString			StrSep;

	for( QSharedPointer<fugio::PinInterface> P : mNode->enumInputPins() )
	{
		if( P->globalId() == mPinInputJoinChar->globalId() )
		{
			continue;
		}

		fugio::PinVariantIterator	String( P );

		for( int i = 0 ; i < String.count() ; i++ )
		{
			StrLst << String.index( i ).toString();
		}
	}

	StrSep = variant( mPinInputJoinChar ).toString();

	QString		StrRes = StrLst.join( StrSep );

	mValOutput->setVariant( StrRes );

	pinUpdated( mPinOutput );
}

QList<QUuid> StringJoinNode::pinAddTypesInput() const
{
	static QList<QUuid> PinLst =
	{
		PID_BOOL,
		PID_BYTEARRAY,
		PID_FLOAT,
		PID_INTEGER,
		PID_STRING,
		PID_VARIANT
	};

	return( PinLst );
}

bool StringJoinNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_OUTPUT );
}

bool StringJoinNode::pinShouldAutoRename( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_INPUT );
}
