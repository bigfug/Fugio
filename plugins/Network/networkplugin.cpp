#include "networkplugin.h"

#include <QCoreApplication>
#include <QNetworkInterface>
#include <QMessageBox>
#include <QTranslator>

#include <fugio/global_interface.h>
#include <fugio/global_signals.h>

#include <fugio/nodecontrolbase.h>

#include <fugio/network/uuid.h>

#include <fugio/editor_interface.h>

#include "getnode.h"

#include "tcpsendnode.h"
#include "tcpreceivenode.h"
#include "tcpsendrawnode.h"
#include "tcpreceiverawnode.h"

#include "udpreceiverawnode.h"
#include "udpsendrawnode.h"

#include "slipdecodenode.h"
#include "slipencodenode.h"

#include "cobsdecodenode.h"
#include "cobsencodenode.h"

#include "websocketdataservernode.h"
#include "websocketclientnode.h"

#include "packetencodenode.h"
#include "packetdecodenode.h"

#include "universesendnode.h"
#include "universereceivenode.h"

QList<QUuid>	NodeControlBase::PID_UUID;

NetworkPlugin *NetworkPlugin::mInstance = 0;

ClassEntry	NodeClasses[] =
{
	ClassEntry( "COBS Decode", "Network", NID_COBS_DECODE, &COBSDecodeNode::staticMetaObject ),
	ClassEntry( "COBS Encode", "Network", NID_COBS_ENCODE, &COBSEncodeNode::staticMetaObject ),
	ClassEntry( "Get", "Network", NID_NETWORK_GET, &GetNode::staticMetaObject ),
	ClassEntry( "Packet Decode", NID_PACKET_DECODE, &PacketDecodeNode::staticMetaObject ),
	ClassEntry( "Packet Encode", NID_PACKET_ENCODE, &PacketEncodeNode::staticMetaObject ),
	ClassEntry( "TCP Send", "Network", NID_TCP_SEND, &TCPSendNode::staticMetaObject ),
	ClassEntry( "TCP Receive", "Network", NID_TCP_RECEIVE, &TCPReceiveNode::staticMetaObject ),
	ClassEntry( "TCP Send Raw", "Network", NID_TCP_SEND_RAW, &TCPSendRawNode::staticMetaObject ),
	ClassEntry( "TCP Receive Raw", "Network", NID_TCP_RECEIVE_RAW, &TCPReceiveRawNode::staticMetaObject ),
	ClassEntry( "UDP Receive Raw", "Network", NID_UDP_RECEIVE_RAW, &UDPReceiveRawNode::staticMetaObject ),
	ClassEntry( "UDP Send Raw", "Network", NID_UDP_SEND_RAW, &UDPSendRawNode::staticMetaObject ),
	ClassEntry( "SLIP Encode", "Network", NID_SLIP_ENCODE, &SLIPEncodeNode::staticMetaObject ),
	ClassEntry( "SLIP Decode", "Network", NID_SLIP_DECODE, &SLIPDecodeNode::staticMetaObject ),
	ClassEntry( "Universe Receieve", "Network", NID_UNIVERSE_SEND, &UniverseReceiveNode::staticMetaObject ),
	ClassEntry( "Universe Send", "Network", NID_UNIVERSE_RECEIVE, &UniverseSendNode::staticMetaObject ),
	ClassEntry( "WebSocket Data Server", "Network", NID_WEBSOCKET_DATA_SERVER, &WebSocketDataServerNode::staticMetaObject ),
	ClassEntry( "WebSocket Client", "Network", NID_WEBSOCKET_CLIENT, &WebSocketClientNode::staticMetaObject ),
	ClassEntry()
};

ClassEntry PinClasses[] =
{
	ClassEntry()
};

NetworkPlugin::NetworkPlugin()
	: mApp( 0 ), mNetworkAccessManager( 0 )
{
	mInstance = this;

	//-------------------------------------------------------------------------
	// Install translator

	static QTranslator		Translator;

	if( Translator.load( QLocale(), QLatin1String( "translations" ), QLatin1String( "_" ), ":/" ) )
	{
		qApp->installTranslator( &Translator );
	}
}

PluginInterface::InitResult NetworkPlugin::initialise( fugio::GlobalInterface *pApp, bool pLastChance )
{
	Q_UNUSED( pLastChance )

	mNetworkAccessManager = new QNetworkAccessManager();

	mApp = pApp;

	mApp->registerNodeClasses( NodeClasses );

	mApp->registerPinClasses( PinClasses );

	fugio::EditorInterface	*EI = qobject_cast<fugio::EditorInterface *>( mApp->findInterface( IID_EDITOR ) );

	if( EI )
	{
		EI->menuAddEntry( fugio::MenuId::HELP, tr( "Network Information..." ), this, SLOT(menuNetworkInformation()) );
	}

	return( INIT_OK );
}

void NetworkPlugin::deinitialise( void )
{
	mApp->unregisterPinClasses( PinClasses );

	mApp->unregisterNodeClasses( NodeClasses );

	mApp = 0;

	if( mNetworkAccessManager )
	{
		delete mNetworkAccessManager;

		mNetworkAccessManager = 0;
	}
}

void NetworkPlugin::menuNetworkInformation()
{
	QMessageBox		InfoBox;
	QStringList		IP4List;
	QStringList		IP6List;

	InfoBox.setIcon( QMessageBox::Information );

	InfoBox.setText( tr( "Network Address Information" ) );

	for( const QHostAddress &HostAddr : QNetworkInterface::allAddresses() )
	{
		QStringList			AddLst;

		if( HostAddr.isLoopback() )
		{
			AddLst << "Loopback";
		}

#if QT_VERSION >= QT_VERSION_CHECK( 5, 6, 0 )
		if( HostAddr.isMulticast() )
		{
			AddLst << "Multicast";
		}
#endif

		if( !HostAddr.scopeId().isEmpty() )
		{
			AddLst << QString( "Scope: %1" ).arg( HostAddr.scopeId() );
		}

		QString				AddStr;

		if( !AddLst.isEmpty() )
		{
			AddStr = QString( " (%1)" ).arg( AddLst.join( ", " ) );
		}

		switch( HostAddr.protocol() )
		{
			case QAbstractSocket::IPv4Protocol:
				IP4List << QString( "%1%2" ).arg( HostAddr.toString() ).arg( AddStr );
				break;

			case QAbstractSocket::IPv6Protocol:
				IP6List << QString( "%1%2" ).arg( HostAddr.toString() ).arg( AddStr );
				break;

			default:
				qDebug() << HostAddr.protocol() << HostAddr.toString();
				break;
		}
	}

	InfoBox.setTextFormat( Qt::RichText );

	QString		InfTxt;

	if( IP4List.isEmpty() && IP6List.isEmpty() )
	{
		InfTxt = tr( "No network interfaces found" );
	}
	else
	{
		if( !IP4List.isEmpty() )
		{
			InfTxt.append( "<p><strong>IP4:</strong></p>" );
			InfTxt.append( "<ul>" );

			for( QString S : IP4List )
			{
				InfTxt.append( "<li>" );
				InfTxt.append( S );
				InfTxt.append( "</li>" );
			}

			InfTxt.append( "</ul>" );
		}

		if( !IP6List.isEmpty() )
		{
			InfTxt.append( "<p><strong>IP6:</strong></p>" );
			InfTxt.append( "<ul>" );

			for( QString S : IP6List )
			{
				InfTxt.append( "<li>" );
				InfTxt.append( S );
				InfTxt.append( "</li>" );
			}

			InfTxt.append( "</ul>" );
		}
	}

	InfTxt.append( QString( "<p><strong>%1:</strong> %2</p>" ).arg( tr( "Local Domain Name" ) ).arg( QHostInfo::localDomainName() ) );

	InfTxt.append( QString( "<p><strong>%1:</strong> %2</p>" ).arg( tr( "Local Host Name" ) ).arg( QHostInfo::localHostName() ) );

	InfoBox.setInformativeText( InfTxt );

	InfoBox.setStandardButtons( QMessageBox::Ok );

	InfoBox.exec();
}
