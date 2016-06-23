#include "encodernode.h"

#include <QtEndian>
#include <QColor>

#include <fugio/context_signals.h>
#include <fugio/core/uuid.h>
#include <fugio/colour/colour_interface.h>

#include <fugio/performance.h>

EncoderNode::EncoderNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_NAMESPACE,	"68A212D5-3BAF-45D2-A184-AF72F1DFC350" );
	FUGID( PIN_INPUT_DATA,		"71F6FA40-5C32-4F29-9BDE-CB54046EB133" );
	FUGID( PIN_OUTPUT_DATA,		"1C1E926C-D89C-40A8-B415-DFD15F75504D" );

	QSharedPointer<fugio::PinInterface>	InpPin1;

	pinInput( "Namespace", InpPin1, PID_OSC_NAMESPACE, PIN_INPUT_NAMESPACE );

	QSharedPointer<fugio::PinInterface>	InpPin2;

	pinInput( "osc", InpPin2, PID_OSC_JOIN, PIN_INPUT_DATA );

	mValOutputData = pinOutput<fugio::VariantInterface *>( "Data", mPinOutputData, PID_BYTEARRAY_LIST, PIN_OUTPUT_DATA );
}

bool EncoderNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	connect( mNode->context()->qobject(), SIGNAL(frameFinalise(qint64)), this, SLOT(contextFrameFinalise(qint64)) );

	return( true );
}

bool EncoderNode::deinitialise()
{
	disconnect( mNode->context()->qobject(), SIGNAL(frameFinalise(qint64)), this, SLOT(contextFrameFinalise(qint64)) );

	return( NodeControlBase::deinitialise() );
}

QList<QUuid> EncoderNode::pinAddTypesInput() const
{
	static QList<QUuid>		PinLst =
	{
		PID_OSC_JOIN,
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

bool EncoderNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_OUTPUT );
}

QUuid EncoderNode::pinAddControlUuid(fugio::PinInterface *pPin) const
{
	if( pPin->direction() == PIN_OUTPUT )
	{
		return( PID_OSC_JOIN );
	}

	return( QUuid() );
}

void EncoderNode::contextFrameFinalise( qint64 pTimeStamp )
{
	if( mDataOutput.isEmpty() )
	{
		return;
	}

	fugio::Performance( mNode, "frameFinalise", pTimeStamp );

	QVariantList	VarLst;

	for( QHash<QString,QVariant>::const_iterator it = mDataOutput.begin() ; it != mDataOutput.end() ; it++ )
	{
		QByteArray		Message;

		switch( QMetaType::Type( it.value().type() ) )
		{
			case QMetaType::QString:
				{
					const QString		&Value = it.value().toString();

					oscMessage( Message, it.key(), "s", Value.toLatin1().constData(), Value.size() + 1 );
				}
				break;

			case QMetaType::Double:
				{
					float		Value = it.value().toDouble();
					quint32		Temp, *TmpPtr;
					quint8		TmpBuf[ 4 ];

					TmpPtr = (quint32 *)&Value;
					Temp   = *TmpPtr;

					qToBigEndian( Temp, TmpBuf );

					oscMessage( Message, it.key(), "f", (const char *)&TmpBuf, 4 );
				}
				break;

			case QMetaType::Float:
				{
					float		Value = it.value().toFloat();
					quint32		Temp, *TmpPtr;
					quint8		TmpBuf[ 4 ];

					TmpPtr = (quint32 *)&Value;
					Temp   = *TmpPtr;

					qToBigEndian( Temp, TmpBuf );

					oscMessage( Message, it.key(), "f", (const char *)&TmpBuf, 4 );
				}
				break;

			case QMetaType::QColor:
				{
					QColor		 C = it.value().value<QColor>();
					quint32		 i;
					quint8		*v = (quint8 *)&i;

					v[ 0 ] = C.red();
					v[ 1 ] = C.green();
					v[ 2 ] = C.blue();
					v[ 3 ] = C.alpha();

					i = qToBigEndian( i );

					oscMessage( Message, it.key(), "r", (const char *)v, 4 );
				}
				break;

			case QMetaType::Int:
				{
					uchar		TmpBuf[ 4 ];

					qToBigEndian( it.value().toInt(), TmpBuf );

					oscMessage( Message, it.key(), "i", (const char *)&TmpBuf, 4 );
				}
				break;

			default:
				continue;
		}

		if( Message.isEmpty() )
		{
			continue;
		}

		VarLst << Message;
	}

	mValOutputData->setVariant( VarLst );

	pinUpdated( mPinOutputData );

	mDataOutput.clear();
}

void EncoderNode::buffer( QByteArray &pArray )
{
	while( pArray.size() % 4 != 0 )
	{
		pArray.append( char( 0x00 ) );
	}
}

void EncoderNode::output( const QString &pAddress, const QVariant &pValue )
{
	mDataOutput.insert( pAddress, pValue );
}

void EncoderNode::addData( QByteArray &pBundle, const QString &pName, const QString &pArgs, const char *pBuffer, qint32 pBufLen)
{
	QByteArray		Packet;

	oscMessage( Packet, pName, pArgs, pBuffer, pBufLen );

	quint32			Temp32 = Packet.size();
	quint8			TmpBuf[ 4 ];

	qToBigEndian( Temp32, TmpBuf );

	pBundle.append( (const char *)&TmpBuf, 4 );		buffer( pBundle );

	pBundle.append( Packet );
}

void EncoderNode::addData( QByteArray &pBundle, const QByteArray &pPacket )
{
	quint32			Temp32 = pPacket.size();
	quint8			TmpBuf[ 4 ];

	qToBigEndian( Temp32, TmpBuf );

	pBundle.append( (const char *)&TmpBuf, 4 );		buffer( pBundle );

	pBundle.append( pPacket );
}

void EncoderNode::oscMessage( QByteArray &pMessage, const QString &pName, const QString &pArgs, const char *pBuffer, qint32 pBufLen )
{
	pMessage.append( pName );
	pMessage.append( char( 0 ) );				buffer( pMessage );
	pMessage.append( ',' );
	pMessage.append( pArgs );
	pMessage.append( char( 0 ) );				buffer( pMessage );
	pMessage.append( pBuffer, pBufLen );		buffer( pMessage );
}

void EncoderNode::inputsUpdated( qint64 pTimeStamp )
{
	fugio::Performance( mNode, "inputsUpdated", pTimeStamp );

	for( QSharedPointer<fugio::PinInterface> P : mNode->enumInputPins() )
	{
		if( !P->isUpdated( pTimeStamp ) )
		{
			continue;
		}

		if( input<fugio::osc::JoinInterface *>( P ) )
		{
			continue;
		}

		fugio::ColourInterface		*C = input<fugio::ColourInterface *>( P );

		if( C )
		{
			output( P->name(), C->colour() );

			continue;
		}

		fugio::VariantInterface	*V = input<fugio::VariantInterface *>( P );

		if( V )
		{
			output( P->name(), V->variant() );

			continue;
		}

		bool				 B;

		int					 I = P->value().toInt( &B );

		if( !B )
		{
			continue;
		}

		output( P->name(), I );
	}
}

void EncoderNode::oscJoin( QStringList pPath, const QVariant &pValue )
{
	QString		FullPath = pPath.join( '/' );

	if( FullPath.isEmpty() )
	{
		return;
	}

	if( !FullPath.startsWith( '/' ) )
	{
		FullPath.prepend( '/' );
	}

	output( FullPath, pValue );
}

void EncoderNode::oscPath( QStringList &pPath ) const
{
	Q_UNUSED( pPath )
}
