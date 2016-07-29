#include "scalenode.h"

#include <fugio/core/uuid.h>

#include <fugio/node_signals.h>

QMap<QString,int>				ScaleNode::mKeyMap;
QMap<QString,ScaleNode::Scale>	ScaleNode::mScaleMap;

ScaleNode::ScaleNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_KEY,	"4e42410b-0707-4ebb-91a7-85d387a61342" );
	FUGID( PIN_INPUT_SCALE,	"b479f1d5-e7d3-4727-b4e1-de9f3309f8c0" );
	FUGID( PIN_INPUT_NOTE,	"5d2071e5-2040-498e-bc65-c45cdd187931" );
	FUGID( PIN_OUTPUT_NOTE,	"364835f7-41ba-449d-bb48-1526395208a8" );

	mValInputKey = pinInput<fugio::ChoiceInterface *>( "Key", mPinInputKey, PID_CHOICE, PIN_INPUT_KEY );

	mValInputScale = pinInput<fugio::ChoiceInterface *>( "Scale", mPinInputScale, PID_CHOICE, PIN_INPUT_SCALE );

	mPinInputNote = pinInput( "Note", PIN_INPUT_NOTE );

	mValOutputNote = pinOutput<fugio::VariantInterface *>( "Note", mPinOutputNote, PID_INTEGER, PIN_OUTPUT_NOTE );

	mNode->pairPins( mPinInputNote, mPinOutputNote );

	if( mKeyMap.isEmpty() )
	{
		static QStringList	KeyLst =
		{
			"C", "C#/Db", "D", "D#/Eb", "E", "F", "F#/Gb", "G", "G#/Ab", "A", "A#/Bb", "B"
		};

		for( int i = 0 ; i < KeyLst.size() ; i++ )
		{
			mKeyMap.insert( KeyLst.at( i ), i );
		}
	}

	mValInputKey->setChoices( mKeyMap.keys() );

	mPinInputKey->setValue( "C" );

	if( mScaleMap.isEmpty() )
	{
		mScaleMap.insert( "Aeolian", AEOLIAN );
		mScaleMap.insert( "Blues", BLUES );
		mScaleMap.insert( "Chromatic", CHROMATIC );
		mScaleMap.insert( "Diatonic Minor", DIATONIC_MINOR );
		mScaleMap.insert( "Dorian", DORIAN );
		mScaleMap.insert( "Harmonic Minor", HARMONIC_MINOR );
		mScaleMap.insert( "Indian", INDIAN );
		mScaleMap.insert( "Locrian", LOCRIAN );
		mScaleMap.insert( "Lydian", LYDIAN );
		mScaleMap.insert( "Major", MAJOR );
		mScaleMap.insert( "Melodic Minor", MELODIC_MINOR );
		mScaleMap.insert( "Minor", MINOR );
		mScaleMap.insert( "Mixolydian", MIXOLYDIAN );
		mScaleMap.insert( "Natural Minor", NATURAL_MINOR );
		mScaleMap.insert( "Pentatonic", PENTATONIC );
		mScaleMap.insert( "Phrygian", PHRYGIAN );
		mScaleMap.insert( "Turkish", TURKISH );
	}

	mValInputScale->setChoices( mScaleMap.keys() );

	mPinInputScale->setValue( "Major" );
}

int ScaleNode::scaleLimit( int NoteOffset, ScaleNode::Scale MidiScale, int BaseNote, int NoteOutput )
{
	switch( MidiScale )
	{
		case AEOLIAN:
		{
			static int	ScaleOffsets[] = { 0, 2, 3, 5, 7, 8, 10 };

			for( int S : ScaleOffsets )
			{
				if( NoteOffset == S )
				{
					NoteOutput = BaseNote + S;

					break;
				}
			}
		}
		break;

		case BLUES:
		{
			static int	ScaleOffsets[] = { 0, 2, 3, 4, 5, 7, 9, 10, 11 };

			for( int S : ScaleOffsets )
			{
				if( NoteOffset == S )
				{
					NoteOutput = BaseNote + S;

					break;
				}
			}
		}
		break;

		case CHROMATIC:
		{
			static int	ScaleOffsets[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

			for( int S : ScaleOffsets )
			{
				if( NoteOffset == S )
				{
					NoteOutput = BaseNote + S;

					break;
				}
			}
		}
		break;

		case DIATONIC_MINOR:
		{
			static int	ScaleOffsets[] = { 0, 2, 3, 5, 7, 8, 10 };

			for( int S : ScaleOffsets )
			{
				if( NoteOffset == S )
				{
					NoteOutput = BaseNote + S;

					break;
				}
			}
		}
		break;

		case DORIAN:
		{
			static int	ScaleOffsets[] = { 0, 2, 3, 5, 7, 9, 10 };

			for( int S : ScaleOffsets )
			{
				if( NoteOffset == S )
				{
					NoteOutput = BaseNote + S;

					break;
				}
			}
		}
		break;

		case HARMONIC_MINOR:
		{
			static int	ScaleOffsets[] = { 0, 2, 3, 5, 7, 8, 11 };

			for( int S : ScaleOffsets )
			{
				if( NoteOffset == S )
				{
					NoteOutput = BaseNote + S;

					break;
				}
			}
		}
		break;

		case INDIAN:
		{
			static int	ScaleOffsets[] = { 0, 1, 1, 4, 5, 8, 10 };

			for( int S : ScaleOffsets )
			{
				if( NoteOffset == S )
				{
					NoteOutput = BaseNote + S;

					break;
				}
			}
		}
		break;

		case LOCRIAN:
		{
			static int	ScaleOffsets[] = { 0, 1, 3, 5, 6, 8, 10 };

			for( int S : ScaleOffsets )
			{
				if( NoteOffset == S )
				{
					NoteOutput = BaseNote + S;

					break;
				}
			}
		}
		break;

		case LYDIAN:
		{
			static int	ScaleOffsets[] = { 0, 2, 4, 6, 7, 9, 10 };

			for( int S : ScaleOffsets )
			{
				if( NoteOffset == S )
				{
					NoteOutput = BaseNote + S;

					break;
				}
			}
		}
		break;

		case MAJOR:
		{
			static int	ScaleOffsets[] = { 0, 2, 4, 5, 7, 9, 11 };

			for( int S : ScaleOffsets )
			{
				if( NoteOffset == S )
				{
					NoteOutput = BaseNote + S;

					break;
				}
			}
		}
		break;

		case MELODIC_MINOR:
		{
			static int	ScaleOffsets[] = { 0, 2, 3, 5, 7, 8, 9, 10, 11 };

			for( int S : ScaleOffsets )
			{
				if( NoteOffset == S )
				{
					NoteOutput = BaseNote + S;

					break;
				}
			}
		}
		break;

		case MINOR:
		{
			static int	ScaleOffsets[] = { 0, 2, 3, 5, 7, 8, 10 };

			for( int S : ScaleOffsets )
			{
				if( NoteOffset == S )
				{
					NoteOutput = BaseNote + S;

					break;
				}
			}
		}
		break;

		case MIXOLYDIAN:
		{
			static int	ScaleOffsets[] = { 0, 2, 4, 5, 7, 9, 10 };

			for( int S : ScaleOffsets )
			{
				if( NoteOffset == S )
				{
					NoteOutput = BaseNote + S;

					break;
				}
			}
		}
		break;

		case NATURAL_MINOR:
		{
			static int	ScaleOffsets[] = { 0, 2, 3, 5, 7, 8, 10 };

			for( int S : ScaleOffsets )
			{
				if( NoteOffset == S )
				{
					NoteOutput = BaseNote + S;

					break;
				}
			}
		}
		break;

		case PENTATONIC:
		{
			static int	ScaleOffsets[] = { 0, 2, 4, 7, 9 };

			for( int S : ScaleOffsets )
			{
				if( NoteOffset == S )
				{
					NoteOutput = BaseNote + S;

					break;
				}
			}
		}
		break;

		case PHRYGIAN:
		{
			static int	ScaleOffsets[] = { 0, 1, 3, 5, 7, 8, 10 };

			for( int S : ScaleOffsets )
			{
				if( NoteOffset == S )
				{
					NoteOutput = BaseNote + S;

					break;
				}
			}
		}
		break;

		case TURKISH:
		{
			static int	ScaleOffsets[] = { 0, 1, 3, 5, 7, 10, 11 };

			for( int S : ScaleOffsets )
			{
				if( NoteOffset == S )
				{
					NoteOutput = BaseNote + S;

					break;
				}
			}
		}
		break;
	}

	return( NoteOutput );
}

void ScaleNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	Scale		MidiScale = mScaleMap.value( variant( mPinInputScale ).toString() );
	int			MidiKey   = mKeyMap.value( variant( mPinInputKey ).toString() );

	for( fugio::NodeInterface::UuidPair UP : mNode->pairedPins() )
	{
		QSharedPointer<fugio::PinInterface>		 PinI = mNode->findPinByLocalId( UP.first );
		QSharedPointer<fugio::PinInterface>		 PinO = mNode->findPinByLocalId( UP.second );

		fugio::VariantInterface					*VarO = qobject_cast<fugio::VariantInterface *>( PinO && PinO->hasControl() ? PinO->control()->qobject() : nullptr );

		if( !PinI || !PinO || !VarO )
		{
			continue;
		}

		int			MidiNote  = qBound( 0, variant( PinI ).toInt(), 127 );

		int			BaseNote = MidiKey;

		// Get into the right octave

		while( MidiNote >= BaseNote + 12 )
		{
			BaseNote += 12;
		}

		int			NoteOffset = MidiNote - BaseNote;
		int			NoteOutput = VarO->variant().toInt();

		NoteOutput = scaleLimit( NoteOffset, MidiScale, BaseNote, NoteOutput );

		NoteOutput = qBound( 0, NoteOutput, 127 );

		if( VarO->variant().toInt() != NoteOutput )
		{
			VarO->setVariant( NoteOutput );

			pinUpdated( PinO );
		}
	}
}


QUuid ScaleNode::pairedPinControlUuid( QSharedPointer<fugio::PinInterface> pPin ) const
{
	return( PID_INTEGER );
}

QList<QUuid> ScaleNode::pinAddTypesInput() const
{
	static QList<QUuid> PinLst =
	{
		PID_INTEGER
	};

	return( PinLst );
}

bool ScaleNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_OUTPUT );
}

