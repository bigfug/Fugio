#include "projectornode.h"

#include <QHostAddress>
#include <QJsonDocument>

#include <fugio/core/uuid.h>
#include <fugio/json/uuid.h>

#include "pjlinkplugin.h"
#include "pjlinkserver.h"

ProjectorNode::ProjectorNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_PROJECTOR, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_POWER,		"cd61d23c-6956-41d9-b0aa-94682255088a" );
	FUGID( PIN_OUTPUT_DATA,		"21836334-9b2c-4502-909c-b84a8d135afa" );

	mValInputProjector = pinInput<fugio::ChoiceInterface *>( tr( "Projector" ), mPinInputProjector, PID_CHOICE, PIN_INPUT_PROJECTOR );

	mPinInputPower = pinInput( tr( "Power" ), PIN_INPUT_POWER );

	mPinInputPower->registerPinInputType( PID_BOOL );

	mValOutputData = pinOutput<fugio::VariantInterface *>( tr( "Data" ), mPinOutputData, PID_JSON, PIN_OUTPUT_DATA );

	clientListUpdated();

	PJLinkServer	*Server = PJLinkPlugin::instance()->server();

	connect( Server, &PJLinkServer::clientListChanged, this, &ProjectorNode::clientListUpdated );
}

void ProjectorNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	PJLinkServer	*Server = PJLinkPlugin::instance()->server();

	QString			 ClientAddressString = variant( mPinInputProjector ).toString();
	QHostAddress	 ClientAddress( ClientAddressString );

	PJLinkClient	*Client = Server->client( ClientAddress );

	if( !Client )
	{
		return;
	}

	if( mPinInputPower->isUpdated( pTimeStamp ) )
	{
		if( variant( mPinInputPower ).toBool() )
		{
			Client->powerOn();
		}
		else
		{
			Client->powerOff();
		}
	}

	QJsonObject		 Projector;

	Projector.insert( "name", Client->name() );

	mValOutputData->setVariant( QJsonDocument( Projector ) );

	pinUpdated( mPinOutputData );
}

void ProjectorNode::clientListUpdated( void )
{
	PJLinkServer	*Server = PJLinkPlugin::instance()->server();

	QVector<QHostAddress>		ClientList = Server->clientAddresses();

	QStringList				ClientLabels;

	for( QHostAddress A : ClientList )
	{
		ClientLabels << A.toString();
	}

	mValInputProjector->setChoices( ClientLabels );
}
