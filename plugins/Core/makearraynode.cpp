#include "makearraynode.h"

#include <QComboBox>
#include <QMatrix4x4>
#include <QSettings>

#include <fugio/core/uuid.h>
#include <fugio/math/uuid.h>
#include <fugio/colour/uuid.h>

#include <fugio/context_interface.h>

MakeArrayNode::MakeArrayNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_FIRST,		"9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_OUTPUT_ARRAY,	"1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );

	mPinInput = pinInput( "Input", PIN_INPUT_FIRST );

	mPinInput->setAutoRename( true );

	mValOutput = pinOutput<fugio::VariantInterface *>( "Output", mPinOutput, PID_VARIANT, PIN_OUTPUT_ARRAY );
}

void MakeArrayNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	QMetaType::Type		CurrType = QMetaType::Type( variant( mPinInput ).userType() );

	if( CurrType == QMetaType::UnknownType )
	{
		return;
	}

	mValOutput->setVariantType( CurrType );

	QList<QSharedPointer<fugio::PinInterface>>	PinLst = mNode->enumInputPins();

	if( mValOutput->variantCount() != PinLst.size() )
	{
		mValOutput->setVariantCount( PinLst.size() );
	}

	if( PinLst.isEmpty() )
	{
		return;
	}

	for( int i = 0 ; i < PinLst.size() ; i++ )
	{
		mValOutput->setVariant( i, variant( PinLst.at( i ) ) );
	}

	pinUpdated( mPinOutput );
}

bool MakeArrayNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	if( pPin->direction() != PIN_OUTPUT )
	{
		return( false );
	}

	fugio::VariantInterface	*V = qobject_cast<fugio::VariantInterface *>( pPin->control()->qobject() );

	return( V );
}

bool MakeArrayNode::pinShouldAutoRename( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_INPUT );
}
