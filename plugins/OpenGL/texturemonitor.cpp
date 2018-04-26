#include "texturemonitor.h"

#include <QDebug>
#include <QOpenGLContext>

#include "texturemonitornode.h"

#include "openglplugin.h"

TextureMonitor::TextureMonitor()
	: mNode( nullptr )
{
	connect( &mDebugLogger, &QOpenGLDebugLogger::messageLogged, this, &TextureMonitor::handleLoggedMessage );

	setAutoFillBackground( false );
}

void TextureMonitor::setNode( TextureMonitorNode *pNode )
{
	mNode = pNode;
}

void TextureMonitor::initializeGL()
{
	initializeOpenGLFunctions();

	OpenGLPlugin::instance()->initGLEW();

#if defined( OPENGL_DEBUG_ENABLE )
	if( mDebugLogger.initialize() )
	{
		mDebugLogger.startLogging( QOpenGLDebugLogger::SynchronousLogging );
	}
#endif

	mNode->node()->context()->nodeInitialised();
}

void TextureMonitor::resizeGL( int w, int h )
{
	initializeOpenGLFunctions();

	glViewport( 0, 0, w, h );
}

void TextureMonitor::paintGL()
{
	initializeOpenGLFunctions();

	if( mNode )
	{
		mNode->paintGL();
	}
}

void TextureMonitor::handleLoggedMessage( const QOpenGLDebugMessage &debugMessage )
{
	qDebug() << mNode->node()->name() << debugMessage;
}
