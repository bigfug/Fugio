#ifndef FFMPEGPLUGIN_H
#define FFMPEGPLUGIN_H

#include <QObject>
#include <QtPlugin>

#include <fugio/global_interface.h>
#include <fugio/plugin_interface.h>

using namespace fugio;

class ffmpegPlugin : public QObject, public fugio::PluginInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::PluginInterface )
	Q_PLUGIN_METADATA( IID "com.bigfug.fugio.ffmpeg.plugin" )

public:
	explicit ffmpegPlugin( void );

	virtual ~ffmpegPlugin( void );

	static ffmpegPlugin *instance( void )
	{
		return( mInstance );
	}

	inline GlobalInterface *app( void )
	{
		return( mApp );
	}

	//-------------------------------------------------------------------------
	// fugio::PluginInterface

	virtual InitResult initialise( fugio::GlobalInterface *pApp, bool pLastChance );

	virtual void deinitialise( void );

	//-------------------------------------------------------------------------

private:
	static ffmpegPlugin		*mInstance;

	GlobalInterface			*mApp;
	ClassEntryList			 mNodeEntries;
	ClassEntryList			 mPinEntries;
};

#endif // FFMPEGPLUGIN_H
