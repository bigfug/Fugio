#include "fftwplugin.h"

#include <QtPlugin>

#include <QDebug>

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
}

FftwPlugin::~FftwPlugin( void )
{
#if defined( FFTW_PLUGIN_SUPPORTED )
	fftwf_cleanup();
#endif
}

PluginInterface::InitResult FftwPlugin::initialise( fugio::GlobalInterface *pApp )
{
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
