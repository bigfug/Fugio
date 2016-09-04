#include "syphonreceivernode.h"

#include <QPushButton>
#include <QInputDialog>

#include <fugio/core/uuid.h>
#include <fugio/syphon/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/context_signals.h>

#include "syphonplugin.h"
#include "syphonpin.h"

SyphonReceiverNode::SyphonReceiverNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_NAME, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );
	FUGID( PIN_OUTPUT_TEXTURE, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_OUTPUT_NAME, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );

	mPinInputName = pinInput( "Name", PIN_INPUT_NAME );

	mValOutputTexture = pinOutput<SyphonPin *>( "Texture", mPinOutputTexture, PID_SYPHON, PIN_OUTPUT_TEXTURE );

	mValOutputName = pinOutput<fugio::VariantInterface *>( "Name", mPinOutputName, PID_STRING, PIN_OUTPUT_NAME );
}

bool SyphonReceiverNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

#if !defined( SYPHON_SUPPORTED )
	if( true )
	{
		mNode->setStatus( fugio::NodeInterface::Error );
		mNode->setStatusMessage( tr( "Syphon not supported on this platform" ) );

		return( false );
	}
#endif

	if( !SyphonPlugin::instance()->hasOpenGLContext() )
	{
		mNode->setStatusMessage( tr( "No active OpenGL context" ) );

		return( false );
	}

	connect( mNode->context()->qobject(), SIGNAL(frameStart()), this, SLOT(onContextFrame()) );

	return( true );
}

bool SyphonReceiverNode::deinitialise()
{
	disconnect( mNode->context()->qobject(), SIGNAL(frameStart()), this, SLOT(onContextFrame()) );

	return( NodeControlBase::deinitialise() );
}

void SyphonReceiverNode::inputsUpdated( qint64 pTimeStamp )
{
	if( mPinOutputName->isUpdated( pTimeStamp ) )
	{
#if defined( SYPHON_SUPPORTED )
		mValOutputTexture->receiver().setServerName( variant( mPinOutputName ).toString().toStdString() );
#endif
	}
}

void SyphonReceiverNode::onContextFrame()
{
#if defined( SYPHON_SUPPORTED )
	if( mValOutputTexture->receiver().serverUuid().empty() )
	{
		std::map<std::string,std::string>	ServerMap = SyphonReceiver::serverList();

		if( !ServerMap.empty() )
		{
			auto	it = ServerMap.begin();

			mValOutputTexture->receiver().setServerUuid( it->first );

			mValOutputName->setVariant( QString::fromStdString( it->second ) );

			pinUpdated( mPinOutputName );
		}
	}

	if( mValOutputTexture->receiver().hasFrameAvailable() )
	{
		mValOutputTexture->updateFromReceiver();

		pinUpdated( mPinOutputTexture );
	}
#endif
}

void SyphonReceiverNode::selectServer()
{
#if defined( SYPHON_SUPPORTED )
	std::map<std::string,std::string>	ServerMap = SyphonReceiver::serverList();
	QStringList							ServerIds;
	QStringList							ServerNames;

	for( auto it = ServerMap.begin() ; it != ServerMap.end() ; it++ )
	{
		QString			ServerId = QString::fromStdString( it->first );
		QString			ServerName = QString::fromStdString( it->second );

		ServerIds.append( ServerId );
		ServerNames.append( ServerName );
	}

	QString			ServerName = variant( mPinOutputName ).toString();
	bool			DialogOk;
	QString			Selection;

	Selection = QInputDialog::getItem( nullptr, tr( "Syphon Server" ), tr( "Choose a Syphon server" ), ServerNames, ServerNames.indexOf( ServerName ), true, &DialogOk );

	if( DialogOk && !Selection.isEmpty() && Selection != ServerName )
	{
		mPinOutputName->setValue( Selection );

		mValOutputTexture->receiver().setServerName( Selection.toStdString() );

		mValOutputName->setVariant( Selection );

		pinUpdated( mPinOutputName );
	}
#endif
}

QWidget *SyphonReceiverNode::gui()
{
	QPushButton		*GUI = new QPushButton( "Select..." );

	connect( GUI, &QPushButton::released, this, &SyphonReceiverNode::selectServer );

	return( GUI );
}
