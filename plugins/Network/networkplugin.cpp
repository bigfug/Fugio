#include "networkplugin.h"

#include <fugio/global_interface.h>
#include <fugio/global_signals.h>

#include <fugio/nodecontrolbase.h>

#include <fugio/network/uuid.h>

#include "tcpsendnode.h"
#include "tcpreceivenode.h"
#include "tcpsendrawnode.h"
#include "tcpreceiverawnode.h"

#include "udpreceiverawnode.h"
#include "udpsendrawnode.h"

#include "slipdecodenode.h"

QList<QUuid>	NodeControlBase::PID_UUID;

NetworkPlugin *NetworkPlugin::mInstance = 0;

ClassEntry	NodeClasses[] =
{
	ClassEntry( "TCP Send", "Network", NID_TCP_SEND, &TCPSendNode::staticMetaObject ),
	ClassEntry( "TCP Receive", "Network", NID_TCP_RECEIVE, &TCPReceiveNode::staticMetaObject ),
	ClassEntry( "TCP Send Raw", "Network", NID_TCP_SEND_RAW, &TCPSendRawNode::staticMetaObject ),
	ClassEntry( "TCP Receive Raw", "Network", NID_TCP_RECEIVE_RAW, &TCPReceiveRawNode::staticMetaObject ),
	ClassEntry( "UDP Receive Raw", "Network", NID_UDP_RECEIVE_RAW, &UDPReceiveRawNode::staticMetaObject ),
	ClassEntry( "UDP Send Raw", "Network", NID_UDP_SEND_RAW, &UDPSendRawNode::staticMetaObject ),
	ClassEntry( "SLIP Decode", "Network", NID_SLIP_DECODE, &SLIPDecodeNode::staticMetaObject ),
	ClassEntry()
};

ClassEntry PinClasses[] =
{
	ClassEntry()
};

PluginInterface::InitResult NetworkPlugin::initialise( fugio::GlobalInterface *pApp )
{
	mApp = pApp;

	mApp->registerNodeClasses( NodeClasses );

	mApp->registerPinClasses( PinClasses );

	return( INIT_OK );
}

void NetworkPlugin::deinitialise( void )
{
	mApp->unregisterPinClasses( PinClasses );

	mApp->unregisterNodeClasses( NodeClasses );

	mApp = 0;
}
