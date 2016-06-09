#include "audioplugin.h"

#include <QtPlugin>

#include <QDebug>

#include <fugio/core/uuid.h>

#include "audiomixernode.h"
#include "signalnode.h"
#include "centroidnode.h"
#include "magnitudenode.h"
#include "frequencybandsnode.h"
#include "audiofilternode.h"
#include "vcfnode.h"

#include "audiopin.h"
#include "fftpin.h"

QList<QUuid>				NodeControlBase::PID_UUID;

ClassEntry		mNodeClasses[] =
{
	ClassEntry( "Centroid", "FFT", NID_CENTROID, &CentroidNode::staticMetaObject ),
	ClassEntry( "Frequency Bands", "FFT", NID_FREQUENCY_BANDS, &FrequencyBandsNode::staticMetaObject ),
	ClassEntry( "Magnitude", "Audio",  NID_AUDIO_MAGNITUDE, &MagnitudeNode::staticMetaObject ),
	ClassEntry( "Mixer", "Audio",  NID_AUDIO_MIXER, &AudioMixerNode::staticMetaObject ),
	ClassEntry( "Signal", "Audio", NID_SIGNAL, &SignalNode::staticMetaObject ),
	ClassEntry( "Filter", "Audio", NID_AUDIO_FILTER, &AudioFilterNode::staticMetaObject ),
	ClassEntry( "VCF", "Audio", NID_VCF, &VCFNode::staticMetaObject ),
	ClassEntry()
};

ClassEntry		mPinClasses[] =
{
	ClassEntry( "Audio", PID_AUDIO, &AudioPin::staticMetaObject ),
	ClassEntry( "FFT",   PID_FFT, &FFTPin::staticMetaObject ),
	ClassEntry()
};

AudioPlugin::AudioPlugin()
	: mApp( 0 )
{
}

AudioPlugin::~AudioPlugin( void )
{
}

PluginInterface::InitResult AudioPlugin::initialise( fugio::GlobalInterface *pApp )
{
	mApp = pApp;

	mApp->registerNodeClasses( mNodeClasses );

	mApp->registerPinClasses( mPinClasses );

	return( INIT_OK );
}

void AudioPlugin::deinitialise()
{
	mApp->unregisterPinClasses( mPinClasses );

	mApp->unregisterNodeClasses( mNodeClasses );
}
