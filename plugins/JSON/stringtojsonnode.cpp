#include "stringtojsonnode.h"

#include <fugio/core/uuid.h>
#include <fugio/json/uuid.h>
#include <fugio/text/uuid.h>

#include <QJsonArray>

#include <fugio/pin_variant_iterator.h>

StringToJsonNode::StringToJsonNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_STRING,	"B8397BEA-CCB5-459B-B952-0C14B1527E0C" );
	FUGID( PIN_OUTPUT_JSON,		"87DF03AA-D3DE-4F44-8201-AB5580585481" );

	mValInputSource = pinInput<fugio::SyntaxErrorInterface *>( "String", mPinInputString, PID_SYNTAX_ERROR, PIN_INPUT_STRING );

	mValOutputJson = pinOutput<fugio::VariantInterface *>( "JSON", mPinOutputJson, PID_JSON, PIN_OUTPUT_JSON );
}

void StringToJsonNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	if( mPinInputString->isUpdated( pTimeStamp ) )
	{
		QJsonParseError	ParseError;

		QJsonDocument	JD = QJsonDocument::fromJson( variant<QString>( mPinInputString ).toLatin1(), &ParseError );

		if( JD.isNull() && ParseError.error != QJsonParseError::NoError )
		{
			mNode->setStatus( fugio::NodeInterface::Error );
			mNode->setStatusMessage( ParseError.errorString() );
		}
		else if( JD != mValOutputJson->variant().toJsonDocument() )
		{
			mNode->setStatus( fugio::NodeInterface::Initialised );

			mValOutputJson->setVariant( JD );

			pinUpdated( mPinOutputJson );
		}
	}
}
