#include "texturemonitor.h"

#include <QDebug>
#include <QOpenGLContext>

#include "texturemonitornode.h"

#include "openglplugin.h"

TextureMonitor::TextureMonitor()
	: mNode( nullptr )
{
	setAutoFillBackground( false );
}

void TextureMonitor::setNode( TextureMonitorNode *pNode )
{
	mNode = pNode;
}

void TextureMonitor::initializeGL()
{
	if( glewExperimental == GL_FALSE )
	{
		glewExperimental = GL_TRUE;

		if( glewInit() != GLEW_OK )
		{
			qWarning() << "GLEW did not initialise";

			return;
		}
	}

	qDebug() << "GL_VENDOR" << QString( (const char *)glGetString( GL_VENDOR ) );

	qDebug() << "GL_RENDERER" << QString( (const char *)glGetString( GL_RENDERER ) );

	qDebug() << "GL_VERSION" << QString( (const char *)glGetString( GL_VERSION ) );

	//qDebug() << context()->extensions();

	switch( context()->format().profile() )
	{
		case QSurfaceFormat::NoProfile:
			qInfo() << "Profile: None";
			break;

		case QSurfaceFormat::CoreProfile:
			qInfo() << "Profile: Core";
			break;

		case QSurfaceFormat::CompatibilityProfile:
			qInfo() << "Profile: Compatibility";
			break;
	}

	qInfo() << "Samples:" << context()->format().samples();
	qInfo() << "Alpha:" << context()->format().alphaBufferSize();
	qInfo() << "Depth:" << context()->format().depthBufferSize();
	qInfo() << "RGB:" << context()->format().redBufferSize() << context()->format().greenBufferSize() << context()->format().blueBufferSize();
}

void TextureMonitor::resizeGL( int w, int h )
{
	glViewport( 0, 0, w, h );
}

void TextureMonitor::paintGL()
{
	if( mNode )
	{
		mNode->paintGL();
	}
}
