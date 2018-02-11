#include "splitlistnode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/core/list_interface.h>
#include <fugio/pin_variant_iterator.h>

#include "coreplugin.h"

SplitListNode::SplitListNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_LIST, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );

	mPinInputList = pinInput( "List/Array", PIN_INPUT_LIST );
}

void SplitListNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	fugio::PinVariantIterator	LstItr( mPinInputList );
	QUuid						LstTyp = CorePlugin::instance()->app()->findPinForMetaType( LstItr.type() );

	QList<QSharedPointer<fugio::PinInterface>>	PinLst = mNode->enumOutputPins();

	for( int i = PinLst.size() ; i < LstItr.count() ; i++ )
	{
		QSharedPointer<fugio::PinInterface>		 NewPin;
		fugio::VariantInterface					*NewVar;

		NewVar = pinOutput<fugio::VariantInterface *>( QString::number( i ), NewPin, LstTyp, QUuid::createUuid() );

		if( NewPin )
		{
			NewPin->setSetting( "i", i );

			PinLst << NewPin;
		}
	}

	for( int i = PinLst.size() ; i > LstItr.count() ; i-- )
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

		if( CurVar && CurIdx >= 0 && CurIdx < LstItr.count() )
		{
			QVariant		NewVar = LstItr.index( CurIdx );

			if( NewVar != CurVar->variant() )
			{
				CurVar->setVariant( NewVar );

				pinUpdated( CurPin );
			}
		}
	}
}
