#ifndef OPENGLPLUGIN_H
#define OPENGLPLUGIN_H

#include "opengl_includes.h"

#include <QObject>
#include <QThread>
#include <QOpenGLTexture>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

#include <fugio/core/uuid.h>
#include <fugio/opengl/uuid.h>
#include <fugio/global_interface.h>
#include <fugio/plugin_interface.h>
#include <fugio/device_factory_interface.h>

#include <fugio/text/syntax_highlighter_factory_interface.h>
#include <fugio/text/syntax_highlighter_instance_interface.h>

using namespace fugio;

class QWindow;

class OpenGLPlugin : public QObject, public PluginInterface, public InterfaceOpenGL, public DeviceFactoryInterface, public fugio::SyntaxHighlighterFactoryInterface, protected QOpenGLFunctions
{
	Q_OBJECT
	Q_INTERFACES( fugio::PluginInterface InterfaceOpenGL fugio::DeviceFactoryInterface fugio::SyntaxHighlighterFactoryInterface )
	Q_PLUGIN_METADATA( IID "com.bigfug.fugio.opengl.plugin" )

public:
	explicit OpenGLPlugin( void );

	virtual ~OpenGLPlugin( void );

	static void parseShaderErrors( QString pErrorText, QList<fugio::SyntaxError> &pErrorData );

	void initGLEW( void );

	static void loadShader( QSharedPointer<fugio::PinInterface> pPin, QOpenGLShaderProgram &pProgram, QOpenGLShader::ShaderType pShaderType, int &pCompiled, int &pFailed );

	//-------------------------------------------------------------------------
	// fugio::PluginInterface

	virtual InitResult initialise( fugio::GlobalInterface *pApp, bool pLastChance ) Q_DECL_OVERRIDE;

	virtual void deinitialise( void ) Q_DECL_OVERRIDE;

	//-------------------------------------------------------------------------
	// InterfaceOpenGL

	virtual void checkErrors( void ) Q_DECL_OVERRIDE;

	virtual void checkErrors( const char *file, int line ) Q_DECL_OVERRIDE;

	virtual void checkErrors( const QString &pContext, const char *file, int line );

	virtual void registerOutputWindowHook( QObject *pObject, const char *pMember ) Q_DECL_OVERRIDE;

	virtual int triangleCount( void ) const Q_DECL_OVERRIDE
	{
		return( mTriangleCount );
	}

	virtual void resetTriangleCount( void ) Q_DECL_OVERRIDE
	{
		mTriangleCount = 0;
	}

	virtual void incrementTriangleCount( int pTriangleCount ) Q_DECL_OVERRIDE
	{
		mTriangleCount += pTriangleCount;
	}

	virtual bool hasContext( void ) Q_DECL_OVERRIDE;

	static QString framebufferError( GLenum pErrorCode );

	//-------------------------------------------------------------------------
	// fugio::DeviceFactoryInterface

	virtual QString deviceConfigMenuText( void ) const Q_DECL_OVERRIDE
	{
		return( "OpenGL Outputs" );
	}

	virtual void deviceConfigGui( QWidget *pParent ) Q_DECL_OVERRIDE;

	//-------------------------------------------------------------------------	
	// SyntaxHighlighterFactoryInterface interface
public:
	virtual SyntaxHighlighterInstanceInterface *syntaxHighlighterInstance( QUuid pUuid ) const Q_DECL_OVERRIDE;

public:
	static QMap<QString,int>				 mMapTargets;
	static QMap<QString,int>				 mMapFormat;
	static QMap<QString,int>				 mMapInternal;
	static QMap<QString,int>				 mMapType;
	static QMap<QString,int>				 mMapFilterMin;
	static QMap<QString,int>				 mMapFilterMag;
	static QMap<QString,int>				 mMapWrap;
	static QMap<QString,int>				 mMapCompare;

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

	bool openWindowFullScreen( void ) const;

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
