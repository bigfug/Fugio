#include "numberarraynode.h"

#include <cmath>

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/core/list_interface.h>

NumberArrayNode::NumberArrayNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_START, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_INCREMENT, "5064e449-8b0b-4447-9009-c81997f754ef" );
	FUGID( PIN_INPUT_END, "5c8f8f4e-58ce-4e47-9e1e-4168d17e1863" );
	FUGID( PIN_OUTPUT_ARRAY, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );

	mPinInputStart = pinInput( "Start", PIN_INPUT_START );

	mPinInputEnd = pinInput( "End", PIN_INPUT_END );

	mPinInputIncrement = pinInput( "Increment", PIN_INPUT_INCREMENT );

	mValOutputArray = pinOutput<fugio::ArrayInterface *>( "Array", mPinOutputArray, PID_ARRAY, PIN_OUTPUT_ARRAY );

	mValOutputArray->setSize( 1 );
	mValOutputArray->setStride( sizeof( float ) );
	mValOutputArray->setType( QMetaType::Float );

	mPinInputIncrement->setValue( 1 );
}

void NumberArrayNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	if( !pTimeStamp )
	{
		return;
	}

	float	s = variant( mPinInputStart ).toReal();
	float	e = variant( mPinInputEnd ).toReal();
	float	i = variant( mPinInputIncrement ).toReal();

	fugio::ListInterface	*L = dynamic_cast<fugio::ListInterface *>( mPinOutputArray->control().data() );

	if( !i )
	{
		return;
	}

	if( ( e > s && i < 0 ) || ( e < s && i > 0 ) )
	{
		return;
	}

	if( e >= s )
	{
		int		c = ( e - s ) / i;

		if( c > 1000 )
		{
			return;
		}

		L->listSetSize( c );

		int		idx = 0;

		for( float j = s ; j < e ; j += i, idx++ )
		{
			L->listSetIndex( idx, j );
		}
	}
	else
	{
		int		c = ( s - e ) / std::abs( i );

		if( c > 1000 )
		{
			return;
		}

		L->listSetSize( c );

		int		idx = 0;

		for( float j = s ; j < e ; j += i, idx++ )
		{
			L->listSetIndex( idx, j );
		}
	}

	pinUpdated( mPinOutputArray );
}
