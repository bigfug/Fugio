#include "firmatanode.h"

#include <QComboBox>
#include <QSpinBox>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDateTime>

#include <fugio/core/uuid.h>
#include <fugio/node_signals.h>
#include <fugio/context_signals.h>

#include <fugio/firmata/firmata_defines.h>

#define DATA_TIMEOUT	(2000)

FirmataNode::FirmataNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mLastResponse( 0 ), mInSysEx( false ), mMessageCount( 0 ),
	  mLastUpdate( 0 )
{
	FUGID( PIN_INPUT_DATA, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_RESET, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_OUTPUT_DATA, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );

	mPinInputTrigger = pinInput( "Trigger", PID_FUGIO_NODE_TRIGGER );

	mPinInputData = pinInput( "Input", PIN_INPUT_DATA );

	mPinInputReset = pinInput( "Reset", PIN_INPUT_RESET );

	mValOutputData = pinOutput<fugio::VariantInterface *>( "Output", mPinOutputData, PID_BYTEARRAY, PIN_OUTPUT_DATA );
}

bool FirmataNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	connect( mNode->context()->qobject(), SIGNAL(frameProcess(qint64)), this, SLOT(contextFrameProcess(qint64)) );
	connect( mNode->context()->qobject(), SIGNAL(frameFinalise(qint64)), this, SLOT(contextFrameFinalise(qint64)) );

	for( QSharedPointer<fugio::PinInterface> P : mNode->enumInputPins() )
	{
		int			PinIdx = P->setting( "firmata-idx", -1 ).toInt();

		if( PinIdx >= 0 && PinIdx <= 127 )
		{
			mPinMapInput.insert( PinIdx, P );
		}
	}

	for( QSharedPointer<fugio::PinInterface> P : mNode->enumOutputPins() )
	{
		fugio::VariantInterface		*V = qobject_cast<fugio::VariantInterface *>( P->hasControl() ? P->control()->qobject() : nullptr );

		if( V )
		{
			int			PinIdx = P->setting( "firmata-idx", -1 ).toInt();

			if( PinIdx >= 0 && PinIdx <= 127 )
			{
				mPinMapOutput.insert( PinIdx, PinOut( P, V ) );
			}
		}
	}

	connect( mNode->qobject(), SIGNAL(pinRemoved(QSharedPointer<fugio::PinInterface>)), this, SLOT(pinRemoved(QSharedPointer<fugio::PinInterface>)) );

	connect( mNode->qobject(), SIGNAL(pinLinked(QSharedPointer<fugio::PinInterface>,QSharedPointer<fugio::PinInterface>)), this, SLOT(pinLinked(QSharedPointer<fugio::PinInterface>,QSharedPointer<fugio::PinInterface>)) );
	connect( mNode->qobject(), SIGNAL(pinUnlinked(QSharedPointer<fugio::PinInterface>,QSharedPointer<fugio::PinInterface>)), this, SLOT(pinUnlinked(QSharedPointer<fugio::PinInterface>,QSharedPointer<fugio::PinInterface>)) );

	return( true );
}

bool FirmataNode::deinitialise()
{
	disconnect( mNode->context()->qobject(), SIGNAL(frameProcess(qint64)), this, SLOT(contextFrameProcess(qint64)) );
	disconnect( mNode->context()->qobject(), SIGNAL(frameFinalise(qint64)), this, SLOT(contextFrameFinalise(qint64)) );

	return( NodeControlBase::deinitialise() );
}

void FirmataNode::processSysEx( const QByteArray &pSysEx )
{
	if( pSysEx[ 0 ] == REPORT_FIRMWARE )		// queryFirmware
	{
		if( pSysEx.size() >= 3 )
		{
			quint8		VerMaj = pSysEx[ 1 ];
			quint8		VerMin = pSysEx[ 2 ];

			QString		FirmwareName;

			for( int i = 3 ; i + 1 < pSysEx.size() ; i += 2 )
			{
				quint8		LSB = pSysEx[ i + 0 ];
				quint8		MSB = pSysEx[ i + 1 ];

				FirmwareName.append( QChar( ushort( MSB ) << 7 | ushort( LSB ) ) );
			}

			qDebug() << "REPORT_FIRMWARE" << VerMaj << VerMin << FirmwareName;

			mOutputCommands.append( START_SYSEX );
			mOutputCommands.append( CAPABILITY_QUERY );
			mOutputCommands.append( END_SYSEX );
		}
	}
	else if( pSysEx[ 0 ] == STRING_DATA )
	{
		QString			String;

		for( int i = 1 ; i + 1 < pSysEx.size() ; i += 2 )
		{
			quint8		LSB = pSysEx[ i + 0 ];
			quint8		MSB = pSysEx[ i + 1 ];

			String.append( QChar( ushort( MSB ) << 7 | ushort( LSB ) ) );
		}

		qDebug() << "STRING_DATA" << String;
	}
	else if( pSysEx[ 0 ] == EXTENDED_ANALOG )
	{
		if( pSysEx.size() >= 3 )
		{
			quint8		Pin = pSysEx[ 1 ];
			int			Val = 0;

			for( int i = 2 ; i < pSysEx.size() ; i++ )
			{
				quint8	Bits = pSysEx[ i ];

				Val |= int( Bits ) << ( ( i - 2 ) * 7 );
			}

			setAnalogValue( Pin, Val );
		}
	}
	else if( pSysEx[ 0 ] == CAPABILITY_RESPONSE )
	{
		mPinMapData.clear();

		quint8		PinIdx = 0;
		quint8		PinMode, PinResolution;

		for( int i = 1 ; i < pSysEx.size() ; )
		{
			quint8	Bits = pSysEx[ i ];

			if( Bits == 0x7f )
			{
				PinIdx++;
				i++;

				continue;
			}

			PinMode = Bits;
			i++;

			if( i < pSysEx.size() )
			{
				PinResolution = pSysEx[ i ];

				i++;

				//qDebug() << "CAPABILITY_RESPONSE" << PinIdx << PinMode << PinResolution;

				mPinMapData.insert( PinIdx, QPair<int,int>( PinMode, PinResolution ) );
			}
		}

		mPinIdxList = mPinMapData.uniqueKeys();

		std::sort( mPinIdxList.begin(), mPinIdxList.end() );

		mOutputCommands.append( START_SYSEX );
		mOutputCommands.append( ANALOG_MAPPING_QUERY );
		mOutputCommands.append( END_SYSEX );
	}
	else if( pSysEx[ 0 ] == ANALOG_MAPPING_RESPONSE )
	{
		mPinAnalogMap = QByteArray( pSysEx.constData() + 1, pSysEx.size() - 1 );

		mPinNames.clear();

		for( int i = 0 ; i < mPinAnalogMap.size() ; i++ )
		{
			if( mPinAnalogMap.size() > i && mPinAnalogMap.at( i ) != 0x7f )
			{
				mPinNames.append( QString( "A%1" ).arg( int( mPinAnalogMap.at( i ) ) ) );
			}
			else
			{
				mPinNames.append( QString( "D%1" ).arg( int( i ) ) );
			}
		}

		beginPinStateQuery();
	}
	else if( pSysEx[ 0 ] == PIN_STATE_RESPONSE )
	{
		if( pSysEx.size() >= 4 )
		{
			quint8			PinIdx = pSysEx[ 1 ];
			quint8			PinMode = pSysEx[ 2 ];
			int				PinState = 0;

			for( int i = 3 ; i < pSysEx.size() ; i++ )
			{
				quint8	Bits = pSysEx[ i ];

				PinState |= int( Bits ) << ( ( i - 2 ) * 7 );
			}

			addPin( PinIdx, PinMode );
		}

		nextPinStateQuery();
	}
}

void FirmataNode::setAnalogValue( int pPinIdx, int pPinVal )
{
	int		AnalogCount = 0;

	for( int i = 0 ; i < mPinAnalogMap.size() ; i++ )
	{
		if( mPinAnalogMap.at( i ) == 0x7f )
		{
			continue;
		}

		if( AnalogCount == pPinIdx )
		{
			pPinIdx = i;

			break;
		}

		AnalogCount++;
	}

	if( mPinMapOutput.contains( pPinIdx ) )
	{
		PinOut		PO = mPinMapOutput.value( pPinIdx );

		if( pPinVal != PO.second->variant().toInt() )
		{
			PO.second->setVariant( pPinVal );

			pinUpdated( PO.first );
		}
	}
}

void FirmataNode::setDigitalValue( int pPinIdx, bool pPinVal )
{
	if( mPinMapOutput.contains( pPinIdx ) )
	{
		PinOut		PO = mPinMapOutput.value( pPinIdx );

		if( pPinVal != PO.second->variant().toBool() )
		{
			PO.second->setVariant( pPinVal );

			pinUpdated( PO.first );
		}
	}
}

void FirmataNode::beginPinStateQuery( void )
{
	mPinStateQueryIdx = 0;

	if( !mPinIdxList.isEmpty() )
	{
		mOutputCommands.append( START_SYSEX );
		mOutputCommands.append( PIN_STATE_QUERY );
		mOutputCommands.append( mPinIdxList.at( mPinStateQueryIdx ) );
		mOutputCommands.append( END_SYSEX );
	}
}

void FirmataNode::nextPinStateQuery( void )
{
	mPinStateQueryIdx++;

	if( mPinIdxList.size() > mPinStateQueryIdx )
	{
		mOutputCommands.append( START_SYSEX );
		mOutputCommands.append( PIN_STATE_QUERY );
		mOutputCommands.append( mPinIdxList.at( mPinStateQueryIdx ) );
		mOutputCommands.append( END_SYSEX );
	}
}

void FirmataNode::pinRemoved(QSharedPointer<fugio::PinInterface> P)
{
	if( P->direction() == PIN_INPUT )
	{
		for( int PinIdx : mPinMapInput.keys() )
		{
			if( mPinMapInput.value( PinIdx )->globalId() == P->globalId() )
			{
				mPinMapInput.remove( PinIdx );

				return;
			}
		}
	}
	else
	{
		for( int PinIdx : mPinMapOutput.keys() )
		{
			if( mPinMapOutput.value( PinIdx ).first->globalId() == P->globalId() )
			{
				mPinMapOutput.remove( PinIdx );

				return;
			}
		}
	}
}

void FirmataNode::pinLinked( QSharedPointer<fugio::PinInterface> S, QSharedPointer<fugio::PinInterface> D )
{
	Q_UNUSED( D )

	if( S->node() == mNode.data() && S->direction() == PIN_OUTPUT )
	{
		for( int PinIdx : mPinMapOutput.keys() )
		{
			PinOut		PO = mPinMapOutput.value( PinIdx );

			if( PO.first->globalId() == S->globalId() )
			{
				if( mPinAnalogMap.at( PinIdx ) == 0x7f )
				{
					mOutputCommands.append( REPORT_DIGITAL_PIN + PinIdx );
				}
				else
				{
					mOutputCommands.append( REPORT_ANALOG_PIN + mPinAnalogMap[ PinIdx ] );
				}

				mOutputCommands.append( char( 1 ) );
				mOutputCommands.append( char( 0 ) );
			}
		}
	}
}

void FirmataNode::pinUnlinked( QSharedPointer<fugio::PinInterface> S, QSharedPointer<fugio::PinInterface> D )
{
	Q_UNUSED( D )

	if( S->node() == mNode.data() && S->direction() == PIN_OUTPUT )
	{
		for( int PinIdx : mPinMapOutput.keys() )
		{
			PinOut		PO = mPinMapOutput.value( PinIdx );

			if( PO.first->globalId() == S->globalId() )
			{
				if( mPinAnalogMap.at( PinIdx ) == 0x7f )
				{
					mOutputCommands.append( REPORT_DIGITAL_PIN + PinIdx );
				}
				else
				{
					mOutputCommands.append( REPORT_ANALOG_PIN + mPinAnalogMap[ PinIdx ] );
				}

				mOutputCommands.append( char( 0 ) );
				mOutputCommands.append( char( 0 ) );
			}
		}
	}
}

void FirmataNode::editPins()
{
	static QStringList	PinTypes =
	{
		"INPUT",
		"OUTPUT",
		"ANALOG",
		"PWM",
		"SERVO",
		"DUMMY",
		"I2C",
		"ONEWIRE",
		"STEPPER",
		"ENCODER",
		"SERIAL",
		"PULLUP"
	};

	QMap<int,QComboBox *>	 SelectionMap;

	QDialog					*Dialog  = new QDialog();

	QFormLayout				*Layout  = new QFormLayout();

	for( int PinIdx : mPinIdxList )
	{
		QComboBox				*PinType = new QComboBox( Dialog );

		SelectionMap.insert( PinIdx, PinType );

		QList<int>				 TypeIds;

		for( QPair<int,int> PinDat : mPinMapData.values( PinIdx ) )
		{
			if( PinDat.first <= 3 )
			{
				TypeIds << PinDat.first;
			}
		}

		std::sort( TypeIds.begin(), TypeIds.end() );

		int						 CurrType = mPinMapType.value( PinIdx, TypeIds.first() );
		int						 CurrIndex = 0;

		for( int TypeId : TypeIds )
		{
			if( CurrType == TypeId )
			{
				CurrIndex = PinType->count();
			}

			if( PinTypes.size() > TypeId )
			{
				PinType->addItem( PinTypes.at( TypeId ) );
			}
		}

		PinType->setCurrentIndex( CurrIndex );

		Layout->addRow( mPinNames.at( PinIdx ), PinType );
	}

	QDialogButtonBox		*ButtonBox = new QDialogButtonBox( Qt::Horizontal, Dialog );

	ButtonBox->addButton( QDialogButtonBox::Ok );
	ButtonBox->addButton( QDialogButtonBox::Cancel );

	connect( ButtonBox, SIGNAL(accepted()), Dialog, SLOT(accept()) );
	connect( ButtonBox, SIGNAL(rejected()), Dialog, SLOT(reject()) );

	QVBoxLayout				*DialogLayout = new QVBoxLayout( Dialog );

	DialogLayout->addLayout( Layout, 1 );
	DialogLayout->addWidget( ButtonBox );

	Dialog->setLayout( DialogLayout );

	if( Dialog->exec() == QDialog::Accepted )
	{
		for( int PinIdx : SelectionMap.keys() )
		{
			QComboBox		*CB = SelectionMap.value( PinIdx );
			int				 Type = PinTypes.indexOf( CB->currentText() );

			if( Type == mPinMapType.value( PinIdx ) )
			{
				continue;
			}

			//qDebug() << PinIdx << PinTypes.at( mPinMapType.value( PinIdx ) ) << "->" << PinTypes.at( Type );

			addPin( PinIdx, Type );

			mOutputCommands.append( SET_PIN_MODE );
			mOutputCommands.append( char( PinIdx ) );
			mOutputCommands.append( char( Type ) );
		}
	}

	delete Dialog;
}

void FirmataNode::contextFrameProcess( qint64 pTimeStamp )
{
	if( mPinInputReset->isUpdated( pTimeStamp ) )
	{
		mInSysEx = false;

		mBuffer.clear();

		mLastResponse = QDateTime::currentMSecsSinceEpoch();

		mOutputCommands.append( PROTOCOL_VERSION );
	}

	if( mPinInputData->isUpdated( pTimeStamp ) )
	{
		QByteArrayList		ByteArrayList;

		mLastResponse = QDateTime::currentMSecsSinceEpoch();

		const QByteArray	&A = variant( mPinInputData ).toByteArray();

		for( quint8 c : A )
		{
			if( mInSysEx )
			{
				if( c == END_SYSEX )
				{
					if( !mBuffer.isEmpty() )
					{
						processSysEx( mBuffer );

						ByteArrayList.append( mBuffer );

						mBuffer.clear();
					}

					mInSysEx = false;
				}
				else
				{
					mBuffer.append( c );
				}
			}
			else if( !mMessageCount )
			{
				if( c == START_SYSEX )
				{
					mInSysEx = true;

					mBuffer.clear();
				}
				else if( ( c & 0x80 ) != 0 )
				{
					if( c >= 0xf0 )
					{
						mMessageType = c;
					}
					else
					{
						mMessageType    = ( c & 0xf0 );
						mMessageChannel = ( c & 0x0f );
					}

					mMessageCount = 1;
				}
			}
			else if( ( c & 0x80 ) != 0 )
			{
				mMessageCount = 0;
			}
			else if( mMessageCount == 1 )
			{
				mMessageData1 = ( c & 0x7f );
				mMessageCount = 2;
			}
			else if( mMessageCount == 2 )
			{
				mMessageData2 = ( c & 0x7f );
				mMessageCount = 3;

				switch( mMessageType )
				{
					case ANALOG_MESSAGE:		// analog I/O message
						{
							setAnalogValue( mMessageChannel, ( mMessageData2 << 7 ) | mMessageData1 );
						}
						break;

					case DIGITAL_MESSAGE:		// digital I/O message
						{
							setDigitalValue( mMessageChannel, ( mMessageData2 << 7 ) | mMessageData1 );
						}
						break;

					case REPORT_ANALOG_PIN:		// report analog pin
						{
						}
						break;

					case REPORT_DIGITAL_PIN:		// report digital port
						{
						}
						break;

					case SET_PIN_MODE:		// set pin mode(I/O)
						{
						}
						break;

					case SET_DIGITAL_VALUE:		// set digital pin value
						{
						}
						break;

					case PROTOCOL_VERSION:		// protocol version
						{
							qDebug() << "Firmata Protocol Version:" << mMessageData1 << mMessageData2;

							mOutputCommands.append( START_SYSEX );
							mOutputCommands.append( CAPABILITY_QUERY );
							mOutputCommands.append( END_SYSEX );
						}
						break;
				}

				mMessageCount = 0;
			}
		}
	}
}

void FirmataNode::contextFrameFinalise( qint64 pTimeStamp )
{
	qint64		TimeStamp = QDateTime::currentMSecsSinceEpoch();

	if( !mLastResponse )
	{
		mLastResponse = TimeStamp - DATA_TIMEOUT;
	}

	if( TimeStamp - mLastResponse >= DATA_TIMEOUT )
	{
//		mOutputCommands.append( PROTOCOL_VERSION );

		mLastResponse = TimeStamp;

		mMessageCount = 0;
	}

	if( !mPinInputTrigger->isConnected() || mPinInputTrigger->isUpdated( pTimeStamp ) )
	{
		for( int PinIdx : mPinMapInput.keys() )
		{
			QSharedPointer<fugio::PinInterface>		P = mPinMapInput.value( PinIdx );

			if( P && P->isUpdated( mLastUpdate ) )
			{
				QVariant		V = variant( P );
				int				PinTyp = mPinMapType.value( PinIdx );

				if( PinTyp == OUTPUT )
				{
					mOutputCommands.append(	SET_DIGITAL_VALUE );
					mOutputCommands.append( char( PinIdx ) );
					mOutputCommands.append(	V.toBool() ? 0x01 : 0x00 );
				}
				else if( PinTyp == PWM )
				{
					const int	i = V.toInt();

					mOutputCommands.append( START_SYSEX );
					mOutputCommands.append( EXTENDED_ANALOG );
					mOutputCommands.append( char( PinIdx ) );
					mOutputCommands.append( ( i >> 0 ) & 0x7f );
					mOutputCommands.append( ( i >> 7 ) & 0x7f );
					mOutputCommands.append( END_SYSEX );
				}
			}
		}

		mLastUpdate = pTimeStamp;
	}

	mValOutputData->setVariant( mOutputCommands );

	if( !mOutputCommands.isEmpty() )
	{
		pinUpdated( mPinOutputData );

		mOutputCommands.clear();
	}
}

void FirmataNode::addPin( int pIdx, int pType )
{
	mPinMapType.insert( pIdx, pType );

	//qDebug() << "PIN_STATE_RESPONSE" << PinIdx << PinMode << PinState;

	QString		PinNam = mPinNames.at( pIdx );

	QSharedPointer<fugio::PinInterface>		P;

	if( pType == OUTPUT || pType == PWM )
	{
		if( true )
		{
			PinMapOutput::iterator	it = mPinMapOutput.find( pIdx );

			if( it != mPinMapOutput.end() )
			{
				mNode->removePin( it.value().first );
			}
		}

		mPinMapOutput.remove( pIdx );

		if( true )
		{
			PinMapInput::iterator	it = mPinMapInput.find( pIdx );

			if( it != mPinMapInput.end() )
			{
				return;
			}
		}

		P = pinInput( PinNam, QUuid::createUuid() );
	}
	else
	{
		QUuid		ControlUuid;

		if( pType == INPUT )
		{
			ControlUuid = PID_BOOL;
		}
		else if( pType == ANALOG )
		{
			ControlUuid = PID_INTEGER;
		}

		if( true )
		{
			PinMapInput::iterator	it = mPinMapInput.find( pIdx );

			if( it != mPinMapInput.end() )
			{
				mNode->removePin( it.value() );
			}
		}

		mPinMapInput.remove( pIdx );

		if( true )
		{
			PinMapOutput::iterator	it = mPinMapOutput.find( pIdx );

			if( it != mPinMapOutput.end() )
			{
				if( it.value().first->controlUuid() == ControlUuid )
				{
					return;
				}

				mNode->removePin( it.value().first );
			}
		}

		mPinMapOutput.remove( pIdx );

		if( pType == INPUT )
		{
			pinOutput<fugio::PinControlInterface *>( PinNam, P, PID_BOOL, QUuid::createUuid() );
		}
		else if( pType == ANALOG )
		{
			pinOutput<fugio::PinControlInterface *>( PinNam, P, PID_INTEGER, QUuid::createUuid() );
		}
	}

	if( P )
	{
		P->setSetting( "firmata-idx", pIdx );

		if( P->direction() == PIN_INPUT )
		{
			mPinMapInput.insert( pIdx, P );
		}
		else
		{
			fugio::VariantInterface		*V = qobject_cast<fugio::VariantInterface *>( P->hasControl() ? P->control()->qobject() : nullptr );

			if( V )
			{
				mPinMapOutput.insert( pIdx, PinOut( P, V ) );
			}
		}
	}
}

QWidget *FirmataNode::gui()
{
	QPushButton		*GUI = new QPushButton( tr( "Edit Pins" ) );

	connect( GUI, SIGNAL(released()), this, SLOT(editPins()) );

	return( GUI );
}
