#include "mediapresetmanager.h"

#if defined( FFMPEG_SUPPORTED )
#include "mediaYouTube240p.h"

#include "mediaYouTube240pWide.h"
#include "mediaYouTube360pWide.h"
#include "mediaYouTube480pWide.h"
#include "mediaYouTube720pWide.h"
#include "mediaYouTube1080pWide.h"

#include "media360_2048.h"
#include "media360_4096.h"

#include "mediaWav44.h"

#include "mediadvdpal.h"
#endif

MediaPresetManager::MediaPresetManager(QObject *parent) :
	QObject(parent)
{
#if defined( FFMPEG_SUPPORTED )
    MediaDvdPal::registerPreset( *this );

	MediaWav44::registerPreset( *this );

	MediaYouTube240p::registerPreset( *this );

	MediaYouTube240pWide::registerPreset( *this );
	MediaYouTube360pWide::registerPreset( *this );
	MediaYouTube480pWide::registerPreset( *this );
	MediaYouTube720pWide::registerPreset( *this );
	MediaYouTube1080pWide::registerPreset( *this );

	Media360_2048::registerPreset( *this );
	Media360_4096::registerPreset( *this );
#endif
}

void MediaPresetManager::registerPreset( const QString &pPresetName, MediaPresetManager::PresetFunction pPresetFunction )
{
	mPresetNames.append( pPresetName );

	mPresetFunctions.insert( pPresetName, pPresetFunction );
}

MediaPresetInterface *MediaPresetManager::preset( const QString &pPresetName ) const
{
	QMap<QString,PresetFunction>::const_iterator	it = mPresetFunctions.find( pPresetName );

	if( it == mPresetFunctions.end() )
	{
		return( 0 );
	}

	PresetFunction		PreFnc = it.value();

	return( PreFnc() );
}

MediaPresetManager *MediaPresetManager::instance()
{
	static MediaPresetManager	*Instance = 0;

	if( Instance == 0 )
	{
		Instance = new MediaPresetManager();
	}

	return( Instance );
}
