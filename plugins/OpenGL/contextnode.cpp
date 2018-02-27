#include "contextnode.h"

#include "openglplugin.h"

#include <QCoreApplication>

#include "openglplugin.h"

ContextNode::ContextNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	connect( &mDebugLogger, &QOpenGLDebugLogger::messageLogged, this, &ContextNode::handleLoggedMessage );
}

bool ContextNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	const bool isGuiThread = QThread::currentThread() == QCoreApplication::instance()->thread();

	if( !isGuiThread )
	{
		qDebug() << "!isGuiThread()";

		return( false );
	}

	if( !mSurface.isValid() )
	{
		mSurface.create();
	}

	if( !mSurface.isValid() )
	{
		qWarning() << "!mSurface.isValid()";

		return( false );
	}

	if( !mContext.create() )
	{
		qWarning() << "!mContext.create()";

		return( false );
	}

	if( !mContext.makeCurrent( &mSurface ) )
	{
		qWarning() << "!mContext.makeCurrent()";

		return( false );
	}

	QOpenGLContext	*Context = QOpenGLContext::currentContext();

	if( !Context )
	{
		qWarning() << "!Context";

		return( false );
	}

	OpenGLPlugin::instance()->initGLEW();

#if defined( OPENGL_DEBUG_ENABLE )
	if( mDebugLogger.initialize() )
	{
		mDebugLogger.startLogging( QOpenGLDebugLogger::SynchronousLogging );
	}
#endif

	if( !OpenGLPlugin::hasContextStatic() )
	{
		return( false );
	}

	return( true );
}

bool ContextNode::deinitialise()
{
	if( !mSurface.isValid() )
	{
		mSurface.destroy();
	}

	return( NodeControlBase::deinitialise() );
}
