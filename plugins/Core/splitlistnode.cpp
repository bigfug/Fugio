#include "splitlistnode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/core/list_interface.h>

SplitListNode::SplitListNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_LIST, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );

	mPinInputList = pinInput( "List/Array", PIN_INPUT_LIST );
}

void SplitListNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	fugio::ListInterface	*LstInf = input<fugio::ListInterface *>( mPinInputList );

	if( LstInf )
	{
		QList<QSharedPointer<fugio::PinInterface>>	PinLst = mNode->enumOutputPins();

		for( int i = PinLst.size() ; i < LstInf->listSize() ; i++ )
		{
			QSharedPointer<fugio::PinInterface>		 NewPin;
			fugio::VariantInterface					*NewVar;

			NewVar = pinOutput<fugio::VariantInterface *>( QString::number( i ), NewPin, LstInf->listPinControl(), QUuid::createUuid() );

			if( NewPin )
			{
				NewPin->setSetting( "i", i );

				PinLst << NewPin;
			}
		}

		for( int i = PinLst.size() ; i > LstInf->listSize() ; i-- )
		{
			QSharedPointer<fugio::PinInterface>		 OldPin;

			for( int j = 0 ; j < PinLst.size() ; j++ )
			{
				OldPin = PinLst[ j ];

				if( i == OldPin->setting( "i", -1 ).toInt() )
				{
					if( !OldPin->isConnected() )
					{
						PinLst.removeAt( j );

						mNode->removePin( OldPin );
					}

					break;
				}
			}
		}

		for( int i = 0 ; i < PinLst.size() ; i++ )
		{
			QSharedPointer<fugio::PinInterface>		 CurPin = PinLst[ i ];
			fugio::VariantInterface					*CurVar = qobject_cast<fugio::VariantInterface *>( CurPin->hasControl() ? CurPin->control()->qobject() : Q_NULLPTR );
			int										 CurIdx = CurPin->setting( "i", -1 ).toInt();

			if( CurVar && CurIdx >= 0 && CurIdx < LstInf->listSize() )
			{
				CurVar->setVariant( LstInf->listIndex( CurIdx ));
			}

		}

		return;
	}
}
