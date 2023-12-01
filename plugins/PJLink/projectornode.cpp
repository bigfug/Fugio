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

	connect( Server, &PJLinkServer::clientUpdated, [=]( PJLinkClient *pClient )
	{
		QString			 ClientAddressString = variant( mPinInputProjector ).toString();
		QHostAddress	 ClientAddress( ClientAddressString );

		PJLinkClient	*Client = Server->client( ClientAddress );

		if( Client != pClient )
		{
			return;
		}

		mNode->context()->updateNode( mNode );
	} );

	connect( Server, &PJLinkServer::clientConnected, [=]( PJLinkClient *pClient )
	{
		QString			 ClientAddressString = variant( mPinInputProjector ).toString();
		QHostAddress	 ClientAddress( ClientAddressString );

		PJLinkClient	*Client = Server->client( ClientAddress );

		if( Client != pClient )
		{
			return;
		}

		mNode->setStatus( fugio::NodeInterface::Initialised );
		mNode->setStatusMessage( tr( "Connected" ) );
	} );

	connect( Server, &PJLinkServer::clientAuthenticationError, [=]( PJLinkClient *pClient )
	{
		QString			 ClientAddressString = variant( mPinInputProjector ).toString();
		QHostAddress	 ClientAddress( ClientAddressString );

		PJLinkClient	*Client = Server->client( ClientAddress );

		if( Client != pClient )
		{
			return;
		}

		mNode->setStatus( fugio::NodeInterface::Error );
		mNode->setStatusMessage( tr( "Password Authentication Failed" ) );
	} );
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
	Projector.insert( "product", Client->productName() );
	Projector.insert( "manufacturer", Client->manufacturer() );
	Projector.insert( "information", Client->information() );
	Projector.insert( "serial", Client->serialNumber() );
	Projector.insert( "filterUsageTime", Client->filterUsageTime() );

	switch( Client->powerStatus() )
	{
		case PJLinkClient::POWER_UNKNOWN:
			Projector.insert( "power", "unknown" );
			break;
		case PJLinkClient::POWER_OFF:
			Projector.insert( "power", "off" );
			break;
		case PJLinkClient::POWER_ON:
			Projector.insert( "power", "on" );
			break;
		case PJLinkClient::COOLING:
			Projector.insert( "power", "cooling" );
			break;
		case PJLinkClient::WARM_UP:
			Projector.insert( "power", "warmup" );
			break;
	}

	switch( Client->inputType() )
	{
		case PJLinkClient::INPUT_UNKNOWN:
			Projector.insert( "inputType", "unknown" );
			break;
		case PJLinkClient::RGB:
			Projector.insert( "inputType", "RGB" );
			break;
		case PJLinkClient::VIDEO:
			Projector.insert( "inputType", "VIDEO" );
			break;
		case PJLinkClient::DIGITAL:
			Projector.insert( "inputType", "DIGITAL" );
			break;
		case PJLinkClient::STORAGE:
			Projector.insert( "inputType", "STORAGE" );
			break;
		case PJLinkClient::NETWORK:
			Projector.insert( "inputType", "NETWORK" );
			break;
		case PJLinkClient::INTERNAL:
			Projector.insert( "inputType", "INTERNAL" );
			break;
	}

	Projector.insert( "input", QString( Client->input() ) );

	QJsonObject		InputResolution;

	InputResolution.insert( "width", Client->inputResolution().width() );
	InputResolution.insert( "height", Client->inputResolution().height() );

	Projector.insert( "inputResolution", InputResolution );

	QJsonObject		RecommendedResolution;

	RecommendedResolution.insert( "width", Client->recommendedResolution().width() );
	RecommendedResolution.insert( "height", Client->recommendedResolution().height() );

	Projector.insert( "recommendedResolution", RecommendedResolution );

	static const QStringList StatusValues =
	{
		"ok",
		"warning",
		"error"
	};

	QJsonObject		Status;

	Status.insert( "fan", StatusValues[ Client->fanStatus() ] );
	Status.insert( "lamp", StatusValues[ Client->lampStatus() ] );
	Status.insert( "temperature", StatusValues[ Client->temperatureStatus() ] );
	Status.insert( "coverOpen", StatusValues[ Client->coverOpenStatus() ] );
	Status.insert( "filter", StatusValues[ Client->filterStatus() ] );
	Status.insert( "other", StatusValues[ Client->otherStatus() ] );

	Projector.insert( "status", Status );

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
