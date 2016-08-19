#ifndef OPENGLPLUGIN_H
#define OPENGLPLUGIN_H

#include "opengl_includes.h"

#include <QObject>
#include <QThread>

#include <fugio/core/uuid.h>
#include <fugio/opengl/uuid.h>
#include <fugio/global_interface.h>
#include <fugio/plugin_interface.h>
#include <fugio/device_factory_interface.h>

//#define OPENGL_DEBUG_ENABLE

#if defined( OPENGL_DEBUG_ENABLE )
#define OPENGL_PLUGIN_DEBUG 	OpenGLPlugin::instance()->checkErrors( __FILE__, __LINE__ );
#define OPENGL_DEBUG(x) 	OpenGLPlugin::instance()->checkErrors( x, __FILE__, __LINE__ );
#else
#define OPENGL_PLUGIN_DEBUG
#define OPENGL_DEBUG(x)
#endif

using namespace fugio;

class QWindow;

class OpenGLPlugin : public QObject, public PluginInterface, public InterfaceOpenGL, public DeviceFactoryInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::PluginInterface InterfaceOpenGL fugio::DeviceFactoryInterface )
	Q_PLUGIN_METADATA( IID "com.bigfug.fugio.opengl.plugin" )

public:
	explicit OpenGLPlugin( void );

	virtual ~OpenGLPlugin( void );

	//-------------------------------------------------------------------------
	// fugio::PluginInterface

	virtual InitResult initialise( fugio::GlobalInterface *pApp, bool pLastChance );

	virtual void deinitialise( void );

	//-------------------------------------------------------------------------
	// InterfaceOpenGL

#if defined( OPENGL_DEBUG_ENABLE )
	virtual void checkErrors( void );

	virtual void checkErrors( const char *file, int line );

	virtual void checkErrors( const QString &pContext, const char *file, int line );
#endif

	virtual void registerOutputWindowHook( QObject *pObject, const char *pMember );

	virtual int triangleCount( void ) const
	{
		return( mTriangleCount );
	}

	virtual void resetTriangleCount( void )
	{
		mTriangleCount = 0;
	}

	virtual void incrementTriangleCount( int pTriangleCount )
	{
		mTriangleCount += pTriangleCount;
	}

	virtual bool hasContext( void );

	static QString framebufferError( GLenum pErrorCode );

	//-------------------------------------------------------------------------
	// fugio::DeviceFactoryInterface

	virtual QString deviceConfigMenuText( void ) const
	{
		return( "OpenGL Outputs" );
	}

	virtual void deviceConfigGui( QWidget *pParent );

	//-------------------------------------------------------------------------

public:
	static QMap<QString,int>				 mMapTargets;
	static QMap<QString,int>				 mMapFormat;
	static QMap<QString,int>				 mMapInternal;
	static QMap<QString,int>				 mMapType;
	static QMap<QString,int>				 mMapFilterMin;
	static QMap<QString,int>				 mMapFilterMag;
	static QMap<QString,int>				 mMapWrap;

	//-------------------------------------------------------------------------

	static inline OpenGLPlugin *instance( void )
	{
		return( mInstance );
	}

	static bool hasContextStatic( void );

	inline static GlobalInterface *app( void )
	{
		return( instance()->mApp );
	}

signals:
	void windowHook( QWindow *pWindow );

private slots:
	void onWindowCreate( QWindow *pWindow );

	void configLoad( QSettings &pSettings );
	void configSave( QSettings &pSettings ) const;

	void globalFrameEnd( void );

	void appAboutToQuit( void );

private:
	void initStaticData();

private:
	static ClassEntry		 mNodeClasses[];
	static ClassEntry		 mPinClasses[];
	static OpenGLPlugin		*mInstance;

	GlobalInterface			*mApp;

	int						 mTriangleCount;
};

#endif // OPENGLPLUGIN_H
