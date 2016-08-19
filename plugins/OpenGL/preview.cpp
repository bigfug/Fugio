#include "preview.h"

#include <QCoreApplication>
#include <QResizeEvent>
#include <QMoveEvent>
#include <QExposeEvent>
#include <QShowEvent>
#include <QHideEvent>

#include "openglplugin.h"
#include "previewnode.h"

#include <fugio/context_interface.h>

Preview::Preview( QSharedPointer<fugio::NodeInterface> pNode )
//	: mNode( pNode ), mContext( 0 ), m_update_pending( false ), mNeedsInitialize( true )
{
//	if( !glfwInit() )
//	{
//		return;
//	}

//	int count;
//	GLFWmonitor** monitors = glfwGetMonitors(&count);

//	for( int i = 0 ; i < count ; i++ )
//	{
//		const GLFWvidmode	* mode = glfwGetVideoMode( monitors[ i ] );

//		qDebug() << mode->width << mode->height << mode->refreshRate;
//	}

//	glfwDefaultWindowHints();

//	glfwWindowHint( GLFW_DECORATED, GL_FALSE );
//	glfwWindowHint( GLFW_FLOATING, GL_TRUE );

//	mWindow = glfwCreateWindow( 640, 480, "My Title", NULL, NULL );

	//glfwSetWindowPos( mWindow, position().x(), position().y() );
}

Preview::~Preview()
{
//	if( mWindow )
//	{
//		glfwDestroyWindow( mWindow );

//		mWindow = nullptr;
//	}
//	if( mContext && OpenGLPlugin::instance() )
//	{
//		OpenGLPlugin::instance()->remContext( mContext );

//		mContext = 0;
//	}

//	if( mLogger != 0 )
//	{
//		mLogger->stopLogging();

//		mLogger->deleteLater();

//		mLogger = 0;
//	}
}

#if 0
bool OpenGLPreview::renderInit()
{
	if( !isExposed() )
	{
		return( false );
	}

	if( mContext == 0 )
	{
		qDebug() << "OpenGLPreview renderInit()";

		if( ( mContext = OpenGLPlugin::instance()->newContext( this ) ) == 0 )
		{
			return( false );
		}

		mContext->setParent( this );
	}

	if( !mContext->makeCurrent( this ) )
	{
		return( false );
	}

	if( mNeedsInitialize )
	{
#if ( QT_VERSION >= QT_VERSION_CHECK( 5, 3, 0 ) )
		if( mContext->format().testOption( QSurfaceFormat::DebugContext ) )
		{
			if( ( mLogger = new QOpenGLDebugLogger( this ) ) != 0 )
			{
				if( mLogger->initialize() ) // initializes in the current context, i.e. ctx
				{
					connect( mLogger, &QOpenGLDebugLogger::messageLogged, OpenGLPlugin::instance(), &OpenGLPlugin::debugMessageLogged );

					mLogger->startLogging();
				}
			}
		}
#endif

		initialize();

		mNeedsInitialize = false;

		QSharedPointer<fugio::NodeInterface>	CurNod = mNode.toStrongRef();

		if( CurNod != 0 )
		{
			CurNod->context()->nodeInitialised();
		}
	}

	return( true );
}

void OpenGLPreview::renderStart( void )
{
	if( !isExposed() )
	{
		return;
	}

	glViewport( 0, 0, width(), height() );

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void OpenGLPreview::renderEnd()
{
	if( !isExposed() )
	{
		return;
	}

	mContext->swapBuffers( this );

	//mContext->doneCurrent();
}

void OpenGLPreview::renderLater()
{
	if( !m_update_pending )
	{
		m_update_pending = true;

		QCoreApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
	}
}

void OpenGLPreview::signalUpdate()
{
//	if( !mContext )
//	{
//		renderInit();
//	}

//	QSharedPointer<fugio::NodeInterface>	CurNod = mNode.toStrongRef();

//	if( CurNod != 0 )
//	{
//		CurNod->context()->updateNode( CurNod );
//	}
}

bool OpenGLPreview::event( QEvent *event)
{
	switch (event->type())
	{
		case QEvent::UpdateRequest:
			m_update_pending = false;
			//signalUpdate();
			renderNow();
			return true;

		default:
			return QWindow::event(event);
	}
}

void OpenGLPreview::resizeEvent( QResizeEvent *pEvent )
{
	Q_UNUSED( pEvent )

	if( isExposed() )
	{
		renderLater();
	}

//	QSharedPointer<fugio::NodeInterface>	CurNod = mNode.toStrongRef();

//	if( CurNod != 0 )
//	{
//		InterfaceOutput		*Output = qobject_cast<InterfaceOutput *>( CurNod->control()->object() );

//		if( Output != 0 )
//		{
//			Output->resize( pEvent->size().width(), pEvent->size().height() );
//		}
//	}

//	signalUpdate();
}

void OpenGLPreview::exposeEvent( QExposeEvent *pEvent )
{
	Q_UNUSED(pEvent);

	if( isExposed() )
	{
		renderLater();
	}

//	if( isExposed() )
//	{
//		if( !mContext )
//		{
//			renderInit();
//		}

//		QSharedPointer<fugio::NodeInterface>	CurNod = mNode.toStrongRef();

//		if( CurNod != 0 )
//		{
//			CurNod->context()->nodeInitialised();
//		}

//		signalUpdate();
//	}
}

void OpenGLPreview::initialize( void )
{
	glClearColor( 0.0, 0.0, 0.0, 0.0 );
}

void OpenGLPreview::renderNow()
{
	QSharedPointer<fugio::NodeInterface>	CurNod = mNode.toStrongRef();

	if( CurNod )
	{
		if( true )
		{
			InterfaceOpenGLRenderer	*N = qobject_cast<InterfaceOpenGLRenderer *>( CurNod->control()->object() );

			if( N )
			{
				N->render();
			}
		}
	}
}

#endif


void Preview::resizeEvent( QResizeEvent *pEvent )
{
	qDebug() << mapToGlobal( QPoint( 0, 0 ) ) << pEvent->size();

//	if( mWindow )
//	{
//		QPoint		P = mapToGlobal( QPoint( 0, 0 ) );

//		glfwSetWindowPos( mWindow, P.x(), P.y() );

//		glfwSetWindowSize( mWindow, pEvent->size().width(), pEvent->size().height() );

//		glfwSwapBuffers( mWindow );
//	}

	QWindow::resizeEvent( pEvent );
}


void Preview::moveEvent( QMoveEvent *pEvent )
{
	qDebug() << mapToGlobal( pEvent->pos() );

//	if( mWindow )
//	{
//		QPoint		P = mapToGlobal( QPoint( 0, 0 ) );

//		glfwSetWindowPos( mWindow, P.x(), P.y() );

//		glfwSwapBuffers( mWindow );
//	}

	QWindow::moveEvent( pEvent );
}


void Preview::exposeEvent( QExposeEvent *pEvent )
{
//	if( mWindow )
//	{
//		if( isExposed() )
//		{
//			glfwShowWindow( mWindow );

//			glfwSwapBuffers( mWindow );
//		}
//		else
//		{
//			glfwHideWindow( mWindow );
//		}
//	}

	QWindow::exposeEvent( pEvent );
}

void Preview::showEvent( QShowEvent *pEvent )
{
	qDebug() << "showEvent";

//	if( mWindow )
//	{
//		glfwShowWindow( mWindow );

//		glfwSwapBuffers( mWindow );
//	}

	QWindow::showEvent( pEvent );
}

void Preview::hideEvent( QHideEvent *pEvent )
{
	qDebug() << "hideEvent";

//	if( mWindow )
//	{
//		glfwHideWindow( mWindow );
//	}

	QWindow::hideEvent( pEvent );
}
