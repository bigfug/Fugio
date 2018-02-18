#include "andnode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>

#include <fugio/pin_variant_iterator.h>

AndNode::AndNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PII_NUMBER1, "c13a41c6-544b-46bb-a9f2-19dd156d236c" );
	FUGID( PII_NUMBER2, "608ac771-490b-4ae6-9c81-12b9af526d09" );
	FUGID( PIN_OUTPUT_BOOL, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );

	pinInput( "Boolean", PII_NUMBER1 );
	pinInput( "Boolean", PII_NUMBER2 );

	mValOutput = pinOutput<fugio::VariantInterface *>( "Boolean", mPinOutput, PID_BOOL, PIN_OUTPUT_BOOL );
}

void AndNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	QList<fugio::PinVariantIterator>	ItrLst;
	int									ItrMax = 0;

	for( QSharedPointer<fugio::PinInterface> P : mNode->enumInputPins() )
	{
		ItrLst << fugio::PinVariantIterator( P );

		ItrMax = std::max( ItrMax, ItrLst.last().count() );
	}

	if( !ItrMax )
	{
		return;
	}

	bool		UpdateOutput = mPinOutput->alwaysUpdate();

	variantSetCount( mValOutput, ItrMax, UpdateOutput );

	for( int i = 0 ; i < ItrMax ; i++ )
	{
		bool		OutVal;

		for( int j = 0 ; j < ItrLst.size() ; j++ )
		{
			bool	NewVal = ItrLst.at( j ).index( i ).toBool();

			OutVal = ( !j ? NewVal : OutVal & NewVal );
		}

		variantSetValue<bool>( mValOutput, i, OutVal, UpdateOutput );
	}

	if( UpdateOutput )
	{
		pinUpdated( mPinOutput );
	}
}

QList<QUuid> AndNode::pinAddTypesInput() const
{
	static QList<QUuid>		PinLst;

	if( PinLst.isEmpty() )
	{
		PinLst << PID_FLOAT;
		PinLst << PID_INTEGER;
		PinLst << PID_STRING;
		PinLst << PID_BOOL;
	}

	return( PinLst );
}

bool AndNode::canAcceptPin(fugio::PinInterface *pPin) const
{
	if( pPin->direction() != PIN_OUTPUT )
	{
		return( false );
	}

	return( true );
}

