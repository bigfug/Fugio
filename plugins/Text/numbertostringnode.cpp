#include "numbertostringnode.h"

#include <fugio/context_interface.h>

NumberToStringNode::NumberToStringNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	static const QUuid	P_NUMBER = QUuid( "{3c2830a1-8b8e-46ca-a8c9-f15d8b3caebd}" );
	static const QUuid	P_BASE   = QUuid( "{c8bee19a-67df-45f0-be17-c41f18fa5129}" );
	static const QUuid	P_WIDTH  = QUuid( "{5d2bf0bf-287f-47d1-b177-0d5be43175b6}" );
	static const QUuid	P_CHAR   = QUuid( "{f61e700b-a5e5-4d04-9369-7c9f96e641fe}" );

	mPinInputNumber    = pinInput( "Number", P_NUMBER );
	mPinInputBase      = pinInput( "Base", P_BASE );
	mPinInputWidth     = pinInput( "Width", P_WIDTH );
	mPinInputCharacter = pinInput( "Character", P_CHAR );

	mPinOutputValue = pinOutput<fugio::VariantInterface *>( "String", mPinOutput, PID_STRING );
}

void NumberToStringNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	int		Number  = 0;
	int		Base    = 10;
	int		Padding = 0;
	QChar	PadChar = QLatin1Char( ' ' );

	Number = variant( mPinInputNumber ).toInt();

	Base = variant( mPinInputBase ).toInt();

	Base = qBound( 2, Base, 36 );

	Padding = variant( mPinInputWidth ).toInt();

	PadChar = variant( mPinInputCharacter ).toChar();

	mPinOutputValue->setVariant( QString( "%1" ).arg( int( Number ), Padding, Base, PadChar ) );

	pinUpdated( mPinOutput );
}
