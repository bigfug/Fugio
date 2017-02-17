#include "intervalnode.h"

#include <fugio/core/uuid.h>

#include <fugio/node_signals.h>

QMap<QString,int>					IntervalNode::mIntervalMap;

IntervalNode::IntervalNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_INTERVAL,	"4e42410b-0707-4ebb-91a7-85d387a61342" );
	FUGID( PIN_INPUT_NOTE,		"5d2071e5-2040-498e-bc65-c45cdd187931" );
	FUGID( PIN_OUTPUT_NOTE,		"364835f7-41ba-449d-bb48-1526395208a8" );

	mValInputInterval = pinInput<fugio::ChoiceInterface *>( "Interval", mPinInputInterval, PID_CHOICE, PIN_INPUT_INTERVAL );

	mPinInputNote = pinInput( "Note", PIN_INPUT_NOTE );

	mValOutputNote = pinOutput<fugio::VariantInterface *>( "Note", mPinOutputNote, PID_INTEGER, PIN_OUTPUT_NOTE );

	static QStringList IntLst =
	{
		"Perfect Unison",
		"Minor Second",
		"Major Second",
		"Minor Third",
		"Major Third",
		"Perfect Fourth",
		"Diminished Fifth",
		"Perfect Fifth",
		"Minor Sixth",
		"Major Sixth",
		"Minor Seventh",
		"Major Seventh",
		"Perfect Octave"
	};

	if( mIntervalMap.isEmpty() )
	{
		for( int i = 0 ; i < IntLst.size() ; i++ )
		{
			mIntervalMap.insert( IntLst.at( i ), i );
		}
	}

	mValInputInterval->setChoices( IntLst );

	mPinInputInterval->setValue( "Perfect Octave" );

	mNode->pairPins( mPinInputNote, mPinOutputNote );
}

void IntervalNode::inputsUpdated( qint64 pTimeStamp )
{
	if( !pTimeStamp )
	{
		return;
	}

	QVariant		V = variant( mPinInputInterval );
	int				Interval = 0;

	switch( QMetaType::Type( V.type() ) )
	{
		case QMetaType::Int:
		case QMetaType::Float:
		case QMetaType::Double:
			Interval = V.toInt();
			break;

		case QMetaType::QString:
			Interval = mIntervalMap.value( V.toString(), V.toInt() );
			break;

		default:
			return;
	}

	for( fugio::NodeInterface::UuidPair UP : mNode->pairedPins() )
	{
		QSharedPointer<fugio::PinInterface>		 PinI = mNode->findPinByLocalId( UP.first );

		if( !PinI || !PinI->isUpdated( pTimeStamp ) )
		{
			continue;
		}

		QSharedPointer<fugio::PinInterface>		 PinO = mNode->findPinByLocalId( UP.second );
		fugio::VariantInterface					*VarO = qobject_cast<fugio::VariantInterface *>( PinO && PinO->hasControl() ? PinO->control()->qobject() : nullptr );

		if( !VarO )
		{
			continue;
		}

		int		Note = variant( mPinInputNote ).toInt();
		int		NoteInterval = Note + Interval;

		NoteInterval = qBound( 0, NoteInterval, 127 );

		//if( VarO->variant().toInt() != NoteInterval )
		{
			VarO->setVariant( NoteInterval );

			pinUpdated( PinO );
		}
	}
}

QUuid IntervalNode::pairedPinControlUuid( QSharedPointer<fugio::PinInterface> pPin ) const
{
	Q_UNUSED( pPin )

	return( PID_INTEGER );
}

QList<QUuid> IntervalNode::pinAddTypesInput() const
{
	static QList<QUuid> PinLst =
	{
		PID_INTEGER
	};

	return( PinLst );
}

bool IntervalNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_OUTPUT );
}

