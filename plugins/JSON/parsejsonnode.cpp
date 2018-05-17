#include "parsejsonnode.h"

#include <QJsonDocument>

#include <fugio/context_interface.h>

#include <fugio/core/uuid.h>
#include <fugio/json/uuid.h>

ParseJsonNode::ParseJsonNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_STRING,	"9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_OUTPUT_JSON, "3d995a98-0f56-4371-a1fa-67ecaefeba1c" );

	mPinInputJson = pinInput( "String", PIN_INPUT_STRING );

	mValOutputJson = pinOutput<fugio::VariantInterface *>( "JSON", mPinOutputJson, PID_JSON, PIN_OUTPUT_JSON );
}

void ParseJsonNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	if( mPinInputJson->isUpdated( pTimeStamp ) )
	{
		QJsonDocument	JD = QJsonDocument::fromJson( variant<QString>( mPinInputJson ).toLatin1() );

		mValOutputJson->setVariant( JD );

		pinUpdated( mPinOutputJson );
	}
}
