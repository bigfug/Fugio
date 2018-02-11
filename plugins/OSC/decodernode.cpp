#include "decodernode.h"

#include <QtEndian>
#include <QColor>

#include <fugio/core/uuid.h>
#include <fugio/osc/split_interface.h>
#include <fugio/colour/colour_interface.h>
#include <fugio/pin_variant_iterator.h>

DecoderNode::DecoderNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_DATA,			"90AABEA7-AF08-49E6-888B-1EB41BA2F2C0" );
	FUGID( PIN_OUTPUT_NAMESPACE,	"6D57F958-E0E2-432D-A6B0-4A16812F500B" );

	mPinInput = pinInput( "Input", PIN_INPUT_DATA );

	mPinInput->registerPinInputTypes( QList<QUuid>{ PID_BYTEARRAY } );

	mValOutputNamespace = pinOutput<NamespacePin *>( "Namespace", mPinOutputNamespace, PID_OSC_NAMESPACE, PIN_OUTPUT_NAMESPACE );
}

void DecoderNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	fugio::PinVariantIterator	Input( mPinInput );

	for( int i = 0 ; i < Input.count() ; i++ )
	{
		const QVariant	Variant = Input.index( i );

		if( Variant.canConvert( QVariant::ByteArray ) )
		{
			processByteArray( Variant.toByteArray() );
		}
	}

	for( QString OscNam : mDataInput.keys() )
	{
		QVariantList	VarLst = mDataInput.value( OscNam );

		if( VarLst.size() == 1 )
		{
			mValOutputNamespace->insert( OscNam, VarLst.first() );
		}
		else
		{
			mValOutputNamespace->insert( OscNam, VarLst );
		}

		QSharedPointer<fugio::PinInterface>		CurPin;

		QStringList		CurLst = OscNam.split( '/', QString::SkipEmptyParts );
		QStringList		NewLst;

		while( !CurPin && !CurLst.isEmpty() )
		{
			QString		CurNam = CurLst.join( '/' ).prepend( '/' );

			CurPin = mNode->findOutputPinByName( CurNam );

			if( !CurPin )
			{
				NewLst.push_front( CurLst.takeLast() );
			}
		}

		if( !CurPin )
		{
			continue;
		}

		if( true )
		{
			fugio::osc::SplitInterface	*II = qobject_cast<fugio::osc::SplitInterface *>( CurPin->hasControl() ? CurPin->control()->qobject() : nullptr );

			if( II )
			{
				II->oscSplit( NewLst, VarLst );

				continue;
			}
		}

		if( true )
		{
			fugio::VariantInterface	*VI = qobject_cast<fugio::VariantInterface *>( CurPin->hasControl() ? CurPin->control()->qobject() : nullptr );

			if( VI )
			{
				if( VarLst.size() == 1 )
				{
					VI->setVariant( VarLst.first() );
				}
				else
				{
					VI->setVariant( VarLst );
				}

				pinUpdated( CurPin );

				continue;
			}
		}
	}

	mDataInput.clear();
}

void DecoderNode::processByteArray( const QByteArray pByteArray )
{
	if( pByteArray.startsWith( "#bundle" ) )
	{
		processBundle( pByteArray );
	}
	else
	{
		processDatagram( pByteArray );
	}
}

void DecoderNode::processDatagram( const QByteArray &pDatagram )
{
	QByteArray		OscAdr;
	int				OscStr = 0;
	int				OscEnd = 0;

	OscEnd = pDatagram.indexOf( (char)0x00, OscStr );
	OscAdr = pDatagram.mid( OscStr, OscEnd - OscStr );
	OscStr = OscEnd + ( 4 - ( OscEnd % 4 ) );

	if( OscAdr.at( 0 ) != '/' )
	{
		return;
	}

	QByteArray		OscArg;

	OscEnd = pDatagram.indexOf( (char)0x00, OscStr );
	OscArg = pDatagram.mid( OscStr, OscEnd - OscStr );
	OscStr = OscEnd + ( 4 - ( OscEnd % 4 ) );

	if( OscArg.at( 0 ) != ',' )
	{
		return;
	}

	OscArg.remove( 0, 1 );

	QVariantList	OscLst;

	/*
	 * Some OSC apps seem to be sending a strange variation on OSC where there
	 * are no arguments, instead the value is the last part of the path
	 *
	 * QuickOSC on Android is one example
	 *
	 * While we don't want to encourage this sort of non-standard behaviour,
	 * we want to be able to account for it so the end user doesn't get
	 * frustrated!
	 *
	 * Also, don't ask me to implement sending these malformed messages!!
	 */

	if( OscArg.isEmpty() )
	{
		QStringList		AdrLst = QString( OscAdr ).split( '/', QString::SkipEmptyParts );

		if( AdrLst.size() >= 2 )
		{
			QString			AdrVal = AdrLst.takeLast();

			OscAdr = QString( "/%1" ).arg( AdrLst.join( '/' ) ).toLatin1();

			bool			FltOk;
			float			FltVal = AdrVal.toFloat( &FltOk );

			if( FltOk )
			{
				OscLst.append( FltVal );
			}
		}
	}

	for( int a = 0 ; a < OscArg.size() ; a++ )
	{
		int		OscInc = 0;

		switch( OscArg.at( a ) )
		{
			case 'i':	// int32
				{
					qint32		v;

					memcpy( &v, pDatagram.data() + OscStr, sizeof( v ) );

					v = qFromBigEndian( v );

					OscLst.append( v );

					OscInc = sizeof( v );
				}
				break;

			case 'f':	// float32
				{
					qint32		v;

					memcpy( &v, pDatagram.data() + OscStr, sizeof( v ) );

					v = qFromBigEndian( v );

					float		*f = (float *)&v;

					OscLst.append( *f );

					OscInc = sizeof( v );
				}
				break;

			case 's':	// OSC-string
			case 'S':	// Alternate type represented as an OSC-string (for example, for systems that differentiate "symbols" from "strings")
				{
					QByteArray		OscVar;

					OscEnd = pDatagram.indexOf( (char)0x00, OscStr );
					OscVar = pDatagram.mid( OscStr, OscEnd - OscStr );
					OscStr = OscEnd + ( 4 - ( OscEnd % 4 ) );

					OscLst.append( QString( OscVar ) );
				}
				break;

			case 'b':	// OSC-blob
				{
					qint32		l;

					memcpy( &l, pDatagram.data() + OscStr, sizeof( l ) );

					l = qFromBigEndian( l );

					OscStr += sizeof( l );

					QByteArray		OscVar;

					OscEnd = OscStr + l;
					OscVar = pDatagram.mid( OscStr, l );
					OscStr = OscEnd + ( 4 - ( OscEnd % 4 ) );

					OscLst.append( OscVar );
				}
				break;

			case 'h':	// 64 bit big-endian two's complement integer
				{
					qlonglong		v;

					memcpy( &v, pDatagram.data() + OscStr, sizeof( v ) );

					v = qFromBigEndian( v );

					OscLst.append( v );

					OscInc = sizeof( v );
				}
				break;

			case 't':	// OSC-timetag
				{
					OscInc = sizeof( qint64 );
				}
				break;

			case 'd':	// 64 bit ("double") IEEE 754 floating point number
				{
					qint64		v;

					memcpy( &v, pDatagram.data() + OscStr, sizeof( v ) );

					v = qFromBigEndian( v );

					double		*f = (double *)&v;

					OscLst.append( *f );

					OscInc = sizeof( v );
				}
				break;

			case 'c':	// an ascii character, sent as 32 bits
				{
					qint32		v;

					memcpy( &v, pDatagram.data() + OscStr, sizeof( v ) );

					v = qFromBigEndian( v );

					OscLst.append( QChar( v ) );

					OscInc = sizeof( v );
				}
				break;

			case 'r':	// 32 bit RGBA color
				{
					qint32		v;

					memcpy( &v, pDatagram.data() + OscStr, sizeof( v ) );

					v = qFromBigEndian( v );

					const quint8 *p = (const quint8 *)&v;

					OscLst.append( QColor( p[ 0 ], p[ 1 ], p[ 2 ], p[ 3 ] ) );

					OscInc = sizeof( v );
				}
				break;

			case 'm':	// 4 byte MIDI message. Bytes from MSB to LSB are: port id, status byte, data1, data2
				{
					OscInc = 4;
				}
				break;

			case 'T':	// True. No bytes are allocated in the argument data.
				OscLst.append( true );
				break;

			case 'F':	// False. No bytes are allocated in the argument data.
				OscLst.append( false );
				break;

			case 'N':	// Nil. No bytes are allocated in the argument data.
				OscLst.append( QVariant() );
				break;

			case 'I':	// Infinitum. No bytes are allocated in the argument data.
				break;

			case '[':	// Indicates the beginning of an array. The tags following are for data in the Array until a close brace tag is reached.
				break;

			case ']':	// Indicates the end of an array.
				break;

			default:
				break;
		}

		OscStr += OscInc;
		OscEnd += OscInc;
	}

	if( OscLst.size() == 1 )
	{
//		QHash<QString,QVariantList>::iterator	it = mDataInput.find( OscAdr );

//		if( it != mDataInput.end() )
//		{
//			it.value().append( OscLst.first() );
//		}
//		else
		{
			QVariantList		VarLst;

			VarLst.append( OscLst.first() );

			mDataInput.insert( OscAdr, VarLst );
		}
	}
	else if( OscLst.size() > 1 )
	{
//		QHash<QString,QVariantList>::iterator	it = mDataInput.find( OscAdr );

//		if( it != mDataInput.end() )
//		{
//			it.value().append( QVariant( OscLst ) );
//		}
//		else
		{
			QVariantList		VarLst;

			VarLst.append( QVariant( OscLst ) );

			mDataInput.insert( OscAdr, VarLst );
		}
	}
}

void DecoderNode::processBundle( const QByteArray &pDatagram )
{
	QByteArray		OscAdr;
	int				OscStr = 0;
	int				OscEnd = 0;

	OscEnd = pDatagram.indexOf( (char)0x00, OscStr );
	OscAdr = pDatagram.mid( OscStr, OscEnd - OscStr );
	OscStr = OscEnd + ( 4 - ( OscEnd % 4 ) );

	quint64		TimeTag;

	memcpy( &TimeTag, pDatagram.data() + OscStr, 8 );

	OscStr += 8;

	TimeTag = qFromBigEndian( TimeTag );

	while( OscStr < pDatagram.size() )
	{
		quint32		ElementSize;

		memcpy( &ElementSize, pDatagram.data() + OscStr, 4 );

		ElementSize = qFromBigEndian( ElementSize );

		OscStr += 4;

		processDatagram( pDatagram.mid( OscStr, ElementSize ) );

		OscStr += ElementSize;
	}
}

QList<fugio::NodeControlInterface::AvailablePinEntry> DecoderNode::availableOutputPins() const
{
	return( mValOutputNamespace->oscPins( QStringList() ) );
}

QList<QUuid> DecoderNode::pinAddTypesOutput() const
{
	static QList<QUuid>	PinLst =
	{
		PID_BOOL,
		PID_COLOUR,
		PID_STRING,
		PID_FLOAT,
		PID_INTEGER,
		PID_BYTEARRAY,
		PID_OSC_SPLIT,
		PID_LIST
	};

	return( PinLst );
}

bool DecoderNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_INPUT );
}

QStringList DecoderNode::oscNamespace()
{
	return( mValOutputNamespace->oscNamespace() );
}

QList<fugio::NodeControlInterface::AvailablePinEntry> DecoderNode::oscPins(const QStringList &pCurDir) const
{
	return( mValOutputNamespace->oscPins( pCurDir ) );
}
