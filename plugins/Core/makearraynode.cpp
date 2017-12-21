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

	mValOutput = pinOutput<fugio::ArrayInterface *>( "Array", mPinOutput, PID_ARRAY, PIN_OUTPUT_ARRAY );
}

void MakeArrayNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	QMetaType::Type		CurrType = QMetaType::Type( variant( mPinInput ).type() );

	if( CurrType == QMetaType::UnknownType )
	{
		return;
	}

	QMetaType	MetaType( CurrType );

	QList<QSharedPointer<fugio::PinInterface>>	PinLst = mNode->enumInputPins();

	if( mValOutput->type() != CurrType )
	{
		mValOutput->setType( CurrType );
		mValOutput->setSize( 1 );

		mValOutput->setStride( MetaType.sizeOf() );
	}

	if( mValOutput->count() != PinLst.size() )
	{
		mValOutput->setCount( PinLst.size() );
	}

	if( PinLst.isEmpty() )
	{
		return;
	}

	quint8		*DstPtr = (quint8 *)mValOutput->array();

	for( QSharedPointer<fugio::PinInterface> P : PinLst )
	{
		QVariant		 V = variant( P );

		if( QMetaType::Type( V.type() ) == CurrType )
		{
			const void		*D = V.constData();

			QMetaType::construct( CurrType, DstPtr, D );
		}
		else
		{
			if( V.convert( CurrType ) )
			{
				const void		*D = V.constData();

				QMetaType::construct( CurrType, DstPtr, D );
			}
		}

		DstPtr += MetaType.sizeOf();
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
