#include "fftwplugin.h"

#include <QtPlugin>

#include <QDebug>

#include <QTranslator>
#include <QApplication>

#include <fugio/fftw/uuid.h>
#include <fugio/nodecontrolbase.h>

#include "fftnode.h"

QList<QUuid>				NodeControlBase::PID_UUID;

ClassEntry		FftwPlugin::mNodeClasses[] =
{
	ClassEntry( "FFT", "FFTW", NID_FFT, &FFTNode::staticMetaObject ),
	ClassEntry()
};

ClassEntry		FftwPlugin::mPinClasses[] =
{
	ClassEntry()
};

FftwPlugin::FftwPlugin( void )
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

FftwPlugin::~FftwPlugin( void )
{
#if defined( FFTW_PLUGIN_SUPPORTED )
	fftwf_cleanup();
#endif
}

PluginInterface::InitResult FftwPlugin::initialise( fugio::GlobalInterface *pApp, bool pLastChance )
{
	Q_UNUSED( pLastChance )

	mApp = pApp;

	mApp->registerNodeClasses( mNodeClasses );

	mApp->registerPinClasses( mPinClasses );

	return( INIT_OK );
}

void FftwPlugin::deinitialise()
{
	mApp->unregisterPinClasses( mPinClasses );

	mApp->unregisterNodeClasses( mNodeClasses );
}
