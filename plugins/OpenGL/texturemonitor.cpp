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
	initializeOpenGLFunctions();

	OpenGLPlugin::instance()->initGLEW();
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
