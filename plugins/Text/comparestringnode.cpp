#include "comparestringnode.h"

#include <fugio/core/uuid.h>
#include <fugio/pin_variant_iterator.h>

CompareStringNode::CompareStringNode( QSharedPointer<fugio::NodeInterface> pNode ) :
	NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_COMPARE,	"AF7205D3-204D-4070-AAD0-3EF106935733" );
	FUGID( PIN_INPUT_CASE,		"eb2445ea-b47d-4a0d-b4fc-ab1fd0229dda" );
	FUGID( PIN_INPUT_SOURCE,	"1900c673-9a10-44c8-9d88-c11cdf5aa441" );
	FUGID( PIN_OUTPUT_RESULT,	"ABDF2527-5590-4FDC-AEC7-E7F93E7E7E89" );

	mPinInputCompare = pinInput( tr( "Compare" ), PIN_INPUT_COMPARE );

	mPinInputCompare->registerPinInputType( PID_STRING );

	mPinInputCaseSensitive = pinInput( tr( "Case Sensitive" ), PIN_INPUT_CASE );

	mPinInputCaseSensitive->registerPinInputType( PID_BOOL );

	mPinInputCaseSensitive->setValue( false );

	mPinInputSource = pinInput( tr( "Input" ), PIN_INPUT_SOURCE );

	mPinInputSource->registerPinInputType( PID_STRING );

	mValOutputResult = pinOutput<fugio::VariantInterface *>( tr( "Results" ), mPinOutputResult, PID_BOOL, PIN_OUTPUT_RESULT );

	mNode->pairPins( mPinInputSource, mPinOutputResult );
}

void CompareStringNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	QString		InputCompare = variant( mPinInputCompare ).toString();
	bool		CaseSensitive = variant( mPinInputCaseSensitive ).toBool();

	for( fugio::NodeInterface::UuidPair UP : mNode->pairedPins() )
	{
		QSharedPointer<fugio::PinInterface>  SrcPin = mNode->findPinByLocalId( UP.first );
		QSharedPointer<fugio::PinInterface>  DstPin = mNode->findPinByLocalId( UP.second );
		fugio::VariantInterface				*DstVar = qobject_cast<fugio::VariantInterface *>( DstPin && DstPin->hasControl() ? DstPin->control()->qobject() : nullptr );

		if( !DstVar )
		{
			continue;
		}

		fugio::PinVariantIterator			 SrcVar( SrcPin );

		bool								 DstUpd = false;

		if( DstVar->variantCount() != SrcVar.count() )
		{
			DstVar->setVariantCount( SrcVar.count() );

			DstUpd = true;
		}

		for( int i = 0 ; i < SrcVar.count() ; i++ )
		{
			QString		NewVar = SrcVar.index( i ).toString();
			bool		StrCmp = InputCompare.compare( NewVar, CaseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive ) == 0;

			if( DstVar->variant( i ).toBool() != StrCmp )
			{
				DstVar->setVariant( i, StrCmp );

				DstUpd = true;
			}
		}

		if( DstUpd )
		{
			pinUpdated( DstPin );
		}
	}
}

QUuid CompareStringNode::pairedPinControlUuid( QSharedPointer<fugio::PinInterface> pPin ) const
{
	return( pPin->direction() == PIN_INPUT ? PID_STRING : PID_STRING );
}

QList<QUuid> CompareStringNode::pinAddTypesInput() const
{
	static QList<QUuid>	PinLst =
	{
		PID_STRING
	};

	return( PinLst );
}

bool CompareStringNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_OUTPUT );
}
