#ifndef FFTWPLUGIN_H
#define FFTWPLUGIN_H

#include <QtPlugin>
#include <QObject>
#include <QIODevice>
#include <QList>

#include <fugio/core/uuid.h>
#include <fugio/global_interface.h>
#include <fugio/plugin_interface.h>

using namespace fugio;

class FftwPlugin : public QObject, public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::PluginInterface )
	Q_PLUGIN_METADATA( IID "com.bigfug.fugio.fftw.plugin" )

public:
	explicit FftwPlugin( void );

	virtual ~FftwPlugin( void );

	//-------------------------------------------------------------------------
	// fugio::PluginInterface

	virtual InitResult initialise( fugio::GlobalInterface *pApp, bool pLastChance );

	virtual void deinitialise( void );

private:
	static ClassEntry		 mNodeClasses[];
	static ClassEntry		 mPinClasses[];

	GlobalInterface			*mApp;
};

#endif // FFTWPLUGIN_H
