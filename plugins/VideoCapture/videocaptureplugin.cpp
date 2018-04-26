#include "videocaptureplugin.h"

#include <QTranslator>
#include <QCoreApplication>

#include <fugio/global_interface.h>
#include <fugio/global_signals.h>

#include <fugio/videocapture/uuid.h>

#include <fugio/nodecontrolbase.h>

#include "videocapturenode.h"

VideoCapturePlugin					*VideoCapturePlugin::mInstance = 0;

QList<QUuid>	NodeControlBase::PID_UUID;

ClassEntry	NodeClasses[] =
{
	ClassEntry( "Video Capture", NID_VIDEOCAPTURE, &VideoCaptureNode::staticMetaObject ),
	ClassEntry()
};

ClassEntry PinClasses[] =
{
	ClassEntry()
};

VideoCapturePlugin::VideoCapturePlugin()
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

QSharedPointer<VideoCaptureDevice> VideoCapturePlugin::device( int pDeviceIndex, int pFormatIndex )
{
	QSharedPointer<VideoCaptureDevice>	S;

	for( QWeakPointer<VideoCaptureDevice> W : mDevices )
	{
		S = W.toStrongRef();

		if( !S )
		{
			continue;
		}

		if( S->deviceIndex() != pDeviceIndex )
		{
			continue;
		}

		if( S->formatIndex() != pFormatIndex )
		{
			return( QSharedPointer<VideoCaptureDevice>() );
		}

		return( S );
	}

	S = QSharedPointer<VideoCaptureDevice>( new VideoCaptureDevice( pDeviceIndex, pFormatIndex ) );

	if( S )
	{
		mDevices << S;
	}

	return( S );
}

PluginInterface::InitResult VideoCapturePlugin::initialise( fugio::GlobalInterface *pApp, bool pLastChance )
{
	Q_UNUSED( pLastChance )

	mApp = pApp;

	mApp->registerNodeClasses( NodeClasses );

	mApp->registerPinClasses( PinClasses );

	return( INIT_OK );
}

void VideoCapturePlugin::deinitialise( void )
{
	mDevices.clear();

	mApp->unregisterPinClasses( PinClasses );

	mApp->unregisterNodeClasses( NodeClasses );

	mApp = 0;
}
