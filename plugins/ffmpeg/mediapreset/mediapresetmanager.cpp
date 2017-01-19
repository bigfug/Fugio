#include "mediapresetmanager.h"

#include "mediaYouTube240p.h"

#include "mediaYouTube240pWide.h"
#include "mediaYouTube360pWide.h"
#include "mediaYouTube480pWide.h"
#include "mediaYouTube720pWide.h"
#include "mediaYouTube1080pWide.h"

#include "mediaWav44.h"

#include "mediadvdpal.h"

MediaPresetManager::MediaPresetManager(QObject *parent) :
	QObject(parent)
{
	MediaDvdPal::registerPreset( *this );

	MediaWav44::registerPreset( *this );

	MediaYouTube240p::registerPreset( *this );

	MediaYouTube240pWide::registerPreset( *this );
	MediaYouTube360pWide::registerPreset( *this );
	MediaYouTube480pWide::registerPreset( *this );
	MediaYouTube720pWide::registerPreset( *this );
	MediaYouTube1080pWide::registerPreset( *this );
}

void MediaPresetManager::registerPreset( const QString &pPresetName, MediaPresetManager::PresetFunction pPresetFunction )
{
	mPresetNames.append( pPresetName );

	mPresetFunctions.insert( pPresetName, pPresetFunction );
}

InterfaceMediaPreset *MediaPresetManager::preset( const QString &pPresetName ) const
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
