#include "texteditorremotenode.h"

#include <QDataStream>
#include <QBuffer>
#include <QByteArray>

#include <fugio/utils.h>

#include <fugio/context_interface.h>
#include <fugio/context_signals.h>

#include <fugio/core/uuid.h>
#include <fugio/text/uuid.h>

#include <fugio/pin_signals.h>

QDataStream &operator <<( QDataStream &DS, const fugio::SyntaxError &SE );
QDataStream &operator >>( QDataStream &DS, fugio::SyntaxError &SE );

TextEditorRemoteNode::TextEditorRemoteNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mLastTime( 0 )
{
	FUGID( PIN_INPUT_PACKETS, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_TEXT, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_OUTPUT_PACKETS, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );
	FUGID( PIN_OUTPUT_TEXT, "249f2932-f483-422f-b811-ab679f006381" );

	mPinInputPackets = pinInput( "Packets", PIN_INPUT_PACKETS );

	mValInputText = pinInput<fugio::SyntaxErrorInterface *>( "Text", mPinInputText, PID_SYNTAX_ERROR, PIN_INPUT_TEXT );

	mValOutputPackets = pinOutput<fugio::VariantInterface *>( "Packets", mPinOutputPackets, PID_BYTEARRAY, PIN_OUTPUT_PACKETS );

	mValOutputText = pinOutput<fugio::VariantInterface *>( "Text", mPinOutputText, PID_STRING, PIN_OUTPUT_TEXT );

	connect( mPinOutputText->qobject(), SIGNAL(linked(QSharedPointer<fugio::PinInterface>)), this, SLOT(textLinked(QSharedPointer<fugio::PinInterface>)) );
	connect( mPinOutputText->qobject(), SIGNAL(unlinked(QSharedPointer<fugio::PinInterface>)), this, SLOT(textUninked(QSharedPointer<fugio::PinInterface>)) );
}

bool TextEditorRemoteNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	connect( mPinOutputText->qobject(), SIGNAL(linked(QSharedPointer<fugio::PinInterface>)), this, SLOT(textLinked(QSharedPointer<fugio::PinInterface>)) );
	connect( mPinOutputText->qobject(), SIGNAL(unlinked(QSharedPointer<fugio::PinInterface>)), this, SLOT(textUninked(QSharedPointer<fugio::PinInterface>)) );

	return( true );
}

bool TextEditorRemoteNode::deinitialise()
{
	disconnect( mPinOutputText->qobject(), SIGNAL(linked(QSharedPointer<fugio::PinInterface>)), this, SLOT(textLinked(QSharedPointer<fugio::PinInterface>)) );
	disconnect( mPinOutputText->qobject(), SIGNAL(unlinked(QSharedPointer<fugio::PinInterface>)), this, SLOT(textUninked(QSharedPointer<fugio::PinInterface>)) );

	return( NodeControlBase::deinitialise() );
}

void TextEditorRemoteNode::loadSettings( QSettings &pSettings )
{
	NodeControlBase::loadSettings( pSettings );
	
	mValInputText->setHighlighterUuid( fugio::utils::string2uuid( pSettings.value( "highlighter", fugio::utils::uuid2string( mValInputText->highlighterUuid() ) ).toString() ) );
}

void TextEditorRemoteNode::saveSettings( QSettings &pSettings ) const
{
	NodeControlBase::saveSettings( pSettings );
	
	pSettings.setValue( "highlighter", fugio::utils::uuid2string( mValInputText->highlighterUuid() ) );
}

void TextEditorRemoteNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	if( mPinInputPackets->isUpdated( pTimeStamp ) )
	{
		processIncomingVariant( variant( mPinInputPackets ) );
	}

	if( mPinInputText->isUpdated( pTimeStamp ) )
	{
		sendText( variant( mPinInputText ).toString() );
	}
}

void TextEditorRemoteNode::processIncomingVariant( const QVariant &V )
{
	if( V.type() == QVariant::List )
	{
		QVariantList		VL = V.toList();

		for( QVariant I : VL )
		{
			processIncomingVariant( I );
		}
	}
	else if( V.type() == QVariant::ByteArray )
	{
		processIncomingPacket( V.toByteArray() );
	}
}

void TextEditorRemoteNode::processIncomingPacket( const QByteArray &B )
{
	QDataStream		DS( B );
	quint16			PacketType = PACKET_UNKNOWN;

	DS >> PacketType;

	switch( PacketType )
	{
		case PACKET_UNKNOWN:
			break;

		case PACKET_TEXT:
			receiveTextPacket( DS );
			break;

		case PACKET_HIGHLIGHTER:
			receiveHighlighter( DS );
			break;

		case PACKET_ERRORS:
			receiveErrors( DS );
			break;
	}
}

void TextEditorRemoteNode::sendText( const QString &pText )
{
	Packet		P( PACKET_TEXT );

	P.stream() << pText;

	appendPacket( P );
}

void TextEditorRemoteNode::sendHighlighter( const QUuid &pUuid )
{
	Packet		P( PACKET_HIGHLIGHTER );

	P.stream() << pUuid;

	appendPacket( P );
}

void TextEditorRemoteNode::receiveTextPacket( QDataStream &DS )
{
	QString		S;

	DS >> S;

	if( S != mValOutputText->variant().toString() )
	{
		mValOutputText->setVariant( S );

		pinUpdated( mPinOutputText );
	}
}

void TextEditorRemoteNode::receiveHighlighter( QDataStream &DS )
{
	QUuid		U;

	DS >> U;

	mValInputText->setHighlighterUuid( U );
}

void TextEditorRemoteNode::receiveErrors( QDataStream &DS )
{
	QList<fugio::SyntaxError> SyntaxErrors;

	DS >> SyntaxErrors;

	mValInputText->setSyntaxErrors( SyntaxErrors );
}

void TextEditorRemoteNode::appendPacket( const TextEditorRemoteNode::Packet &P )
{
	if( P.isEmpty() )
	{
		return;
	}

	qint64		CurrTime = mNode->context()->global()->timestamp();

	if( mLastTime < CurrTime )
	{
		mValOutputPackets->variantClear();

		mLastTime = CurrTime;
	}

	mValOutputPackets->variantAppend( P.array() );

	pinUpdated( mPinOutputPackets );
}

void TextEditorRemoteNode::textLinked( QSharedPointer<fugio::PinInterface> pPin )
{
	fugio::SyntaxErrorInterface	*SEI = ( pPin->hasControl() ? qobject_cast<fugio::SyntaxErrorInterface *>( pPin->control()->qobject() ) : nullptr );

	if( SEI )
	{
		connect( SEI->syntaxErrorSignals(), SIGNAL(syntaxErrorsUpdated(QList<fugio::SyntaxError>)), this, SLOT(syntaxErrorsUpdated(QList<fugio::SyntaxError>)) );

		sendHighlighter( SEI->highlighterUuid() );
		
		syntaxErrorsUpdated( SEI->syntaxErrors() );
	}
}

void TextEditorRemoteNode::textUninked( QSharedPointer<fugio::PinInterface> pPin )
{
	fugio::SyntaxErrorInterface	*SEI = ( pPin->hasControl() ? qobject_cast<fugio::SyntaxErrorInterface *>( pPin->control()->qobject() ) : nullptr );

	if( SEI )
	{
		connect( SEI->syntaxErrorSignals(), SIGNAL(syntaxErrorsUpdated(QList<fugio::SyntaxError>)), this, SLOT(syntaxErrorsUpdated(QList<fugio::SyntaxError>)) );
	}
}

void TextEditorRemoteNode::syntaxErrorsUpdated( QList<fugio::SyntaxError> pSyntaxErrors )
{
	Packet		P( PACKET_ERRORS );

	P.stream() << pSyntaxErrors;

	appendPacket( P );
}
