#include "spoutplugin.h"

#include <QTranslator>
#include <QApplication>

#include <QTimer>

#include <fugio/global_interface.h>
#include <fugio/global_signals.h>

#include <fugio/nodecontrolbase.h>

#include <fugio/spout/uuid.h>

#if defined( SPOUT_SUPPORTED )
#include <Spout.h>
#endif

#include "spoutreceivernode.h"
#include "spoutsendernode.h"
#include <fugio/opengl/uuid.h>

#include "spoutpin.h"

using namespace fugio;

SpoutPlugin		*SpoutPlugin::mInstance = 0;

QList<QUuid>	NodeControlBase::PID_UUID;

ClassEntry	NodeClasses[] =
{
	ClassEntry( "Spout Receiver", "Spout", ClassEntry::None, NID_SPOUT_RECEIVER, &SpoutReceiverNode::staticMetaObject ),
	ClassEntry( "Spout Sender", "Spout", ClassEntry::None, NID_SPOUT_SENDER, &SpoutSenderNode::staticMetaObject ),
	ClassEntry()
};

ClassEntry PinClasses[] =
{
	ClassEntry( "Spout", PID_SPOUT, &SpoutPin::staticMetaObject ),
	ClassEntry()
};

SpoutPlugin::SpoutPlugin() : mApp( 0 )
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

bool SpoutPlugin::hasOpenGLContext()
{
	InterfaceOpenGL		*OGL = qobject_cast<InterfaceOpenGL *>( mApp->findInterface( IID_OPENGL ) );

	return( OGL && OGL->hasContext() );
}

bool SpoutPlugin::hasOpenGLContextStatic()
{
	return( instance()->hasOpenGLContext() );
}

fugio::PluginInterface::InitResult SpoutPlugin::initialise( fugio::GlobalInterface *pApp, bool pLastChance )
{
	Q_UNUSED( pLastChance )

	if( !pApp->findInterface( IID_OPENGL ) )
	{
		return( INIT_DEFER );
	}

	mApp = pApp;

	mApp->registerNodeClasses( NodeClasses );

	mApp->registerPinClasses( PinClasses );

	updateReceiverList();

	return( INIT_OK );
}

void SpoutPlugin::deinitialise( void )
{
	mApp->unregisterPinClasses( PinClasses );

	mApp->unregisterNodeClasses( NodeClasses );

	mApp = 0;
}

void SpoutPlugin::updateReceiverList()
{
#if defined( SPOUT_SUPPORTED )
	SpoutReceiver	Receiver;
	QStringList		NameList;

	int				NameCount = Receiver.GetSenderCount();

	for( int i = 0 ; i < NameCount ; i++ )
	{
		char		Name[ 256 ];

		if( Receiver.GetSenderName( i, Name ) )
		{
			NameList << QString( Name );
		}
	}

	if( NameList.size() != mNameList.size() )
	{
		mNameList = NameList;

		emit receiverList( mNameList );
	}
	else
	{
		bool		Diff = false;

		for( int i = 0 ; i < NameCount && !Diff ; i++ )
		{
			if( NameList.at( i ) != mNameList.at( i ) )
			{
				Diff = true;
			}
		}

		if( Diff )
		{
			mNameList = NameList;

			emit receiverList( mNameList );
		}
	}

	QTimer::singleShot( 1000, this, SLOT(updateReceiverList()) );
#endif
}
