#ifndef LEAPMOTIONPLUGIN_H
#define LEAPMOTIONPLUGIN_H

#include <QObject>
#include <QVector3D>

#include <fugio/core/uuid.h>
#include <fugio/global_interface.h>
#include <fugio/plugin_interface.h>

#if defined( LEAP_PLUGIN_SUPPORTED )
#include <Leap.h>
#endif

class LeapMotionPlugin : public QObject, public fugio::PluginInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::PluginInterface )
	Q_PLUGIN_METADATA( IID "com.bigfug.fugio.leap.plugin" )

public:
	explicit LeapMotionPlugin( void );

	virtual ~LeapMotionPlugin( void );

#if defined( LEAP_PLUGIN_SUPPORTED )
	static QVector3D leapToVec3( const Leap::Vector &pSrcVec );
#endif

	//-------------------------------------------------------------------------
	// fugio::PluginInterface

	virtual InitResult initialise( fugio::GlobalInterface *pApp, bool pLastChance );

	virtual void deinitialise( void );

private:
	static fugio::ClassEntry		 mNodeClasses[];
	static fugio::ClassEntry		 mPinClasses[];

	fugio::GlobalInterface			*mApp;
};

#endif // LEAPMOTIONPLUGIN_H
