#ifndef KINECTPLUGIN_H
#define KINECTPLUGIN_H

#include <QObject>

#include <fugio/core/uuid.h>
#include <fugio/global_interface.h>
#include <fugio/plugin_interface.h>

class KinectPlugin : public QObject, public fugio::PluginInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::PluginInterface )
	Q_PLUGIN_METADATA( IID "com.bigfug.fugio.kinect.plugin" )

public:
	explicit KinectPlugin( void );
	
	virtual ~KinectPlugin( void );

	//-------------------------------------------------------------------------
	// fugio::PluginInterface

	virtual InitResult initialise( fugio::GlobalInterface *pApp, bool pLastChance );

	virtual void deinitialise( void );

	//-------------------------------------------------------------------------

private:
	fugio::GlobalInterface	*mApp;
	fugio::ClassEntryList	 mNodeEntries;
	fugio::ClassEntryList	 mPinEntries;
};

#endif // KINECTPLUGIN_H
