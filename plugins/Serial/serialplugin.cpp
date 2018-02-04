#include "serialplugin.h"

#include <QTranslator>
#include <QApplication>

#include "deviceserialconfiguration.h"

#include "serialinputnode.h"
#include "serialoutputnode.h"

#include "serialencodernode.h"
#include "serialdecodernode.h"

#include <fugio/global_signals.h>

QList<QUuid>				fugio::NodeControlBase::PID_UUID;

fugio::ClassEntry		SerialPlugin::mNodeClasses[] =
{
	fugio::ClassEntry( "Serial Input", "Serial", NID_SERIAL_INPUT, &SerialInputNode::staticMetaObject ),
	fugio::ClassEntry( "Serial Output", "Serial", NID_SERIAL_OUTPUT, &SerialOutputNode::staticMetaObject ),
	fugio::ClassEntry( "Serial Encoder", "Serial", NID_SERIAL_ENCODER, &SerialEncoderNode::staticMetaObject ),
	fugio::ClassEntry( "Serial Decoder", "Serial", NID_SERIAL_DECODER, &SerialDecoderNode::staticMetaObject ),
	fugio::ClassEntry()
};

SerialPlugin::SerialPlugin( void )
	: mApp( 0 )
{
	//-------------------------------------------------------------------------
	// Install translator

	static QTranslator		Translator;

	if( Translator.load( QLocale(), QLatin1String( "translations" ), QLatin1String( "_" ), ":/" ) )
	{
		qApp->installTranslator( &Translator );
	}
}

SerialPlugin::~SerialPlugin()
{

}

fugio::PluginInterface::InitResult SerialPlugin::initialise( fugio::GlobalInterface *pApp, bool pLastChance )
{
	Q_UNUSED( pLastChance )

	mApp = pApp;

	mApp->registerNodeClasses( mNodeClasses );

	DeviceSerial::deviceInitialise();

	mApp->registerDeviceFactory( this );

	connect( mApp->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(onGlobalFrameStart(qint64)) );
	connect( mApp->qobject(), SIGNAL(frameEnd()), this, SLOT(onGlobalFrameEnd()) );

	connect( mApp->qobject(), SIGNAL(configLoad(QSettings&)), this, SLOT(configLoad(QSettings&)) );
	connect( mApp->qobject(), SIGNAL(configSave(QSettings&)), this, SLOT(configSave(QSettings&)) );

	return( INIT_OK );
}

void SerialPlugin::deinitialise()
{
	mApp->unregisterDeviceFactory( this );

	DeviceSerial::deviceDeinitialise();

	mApp->unregisterNodeClasses( mNodeClasses );
}

void SerialPlugin::deviceConfigGui( QWidget *pParent )
{
	DeviceSerialConfiguration	*CFG = new DeviceSerialConfiguration( pParent );

	CFG->exec();
}

void SerialPlugin::onGlobalFrameStart( qint64 pTimeStamp )
{
	DeviceSerial::devicePacketStart( pTimeStamp );
}

void SerialPlugin::onGlobalFrameEnd( void )
{
	DeviceSerial::devicePacketEnd();
}

void SerialPlugin::configLoad(QSettings &pSettings)
{
	DeviceSerial::deviceCfgLoad( pSettings );
}

void SerialPlugin::configSave(QSettings &pSettings) const
{
	DeviceSerial::deviceCfgSave( pSettings );
}
