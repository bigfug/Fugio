#ifndef OCULUSRIFTPLUGIN_H
#define OCULUSRIFTPLUGIN_H

#include <fugio/plugin_interface.h>
#include <fugio/device_factory_interface.h>
#include <fugio/global_interface.h>

#if defined( OCULUS_PLUGIN_SUPPORTED )
#include <OVR_CAPI.h>
#endif

#define OPENGL_PLUGIN_DEBUG 	OpenGLPlugin::instance()->checkErrors( __FILE__, __LINE__ );

using namespace fugio;

class OculusRiftPlugin : public QObject, public fugio::PluginInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::PluginInterface )
	Q_PLUGIN_METADATA( IID "com.bigfug.fugio.oculus-rift.plugin" )

public:
	explicit OculusRiftPlugin( void );

	virtual ~OculusRiftPlugin( void ) {}

	bool hasOpenGLContext();

	//-------------------------------------------------------------------------
	// fugio::PluginInterface

	virtual InitResult initialise( fugio::GlobalInterface *pApp, bool pLastChance );

	virtual void deinitialise( void );

	//-------------------------------------------------------------------------
	// InterfaceDeviceFactory

//	virtual QString deviceConfigMenuText( void ) const
//	{
//		return( "Oculus Rift" );
//	}

//	virtual void deviceConfigGui( QWidget *pParent );

	//-------------------------------------------------------------------------

	static OculusRiftPlugin *instance( void )
	{
		return( mInstance );
	}

	fugio::GlobalInterface *global( void ) const
	{
		return( mApp );
	}

private slots:
	void globalFrameStart( qint64 pTimeStamp );

private:
	static ClassEntry		 mNodeClasses[];
	static ClassEntry		 mPinClasses[];

	static OculusRiftPlugin	*mInstance;

	fugio::GlobalInterface		*mApp;
};

#endif // OCULUSRIFTPLUGIN_H
