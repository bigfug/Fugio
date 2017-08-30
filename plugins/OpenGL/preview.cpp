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

Preview::Preview( QWeakPointer<fugio::NodeInterface> pNode, QWidget *pParent )
	: QOpenGLWidget( pParent ), mNode( pNode )
{
}

Preview::~Preview()
{
}

void Preview::initializeGL()
{
	QSharedPointer<fugio::NodeInterface>	NI = mNode.toStrongRef();

	if( NI )
	{
		NI->context()->nodeInitialised();
	}

	OpenGLPlugin::instance()->initGLEW();
}

void Preview::resizeGL( int w, int h )
{
}

void Preview::paintGL()
{
	QSharedPointer<fugio::NodeInterface>	NI = mNode.toStrongRef();

	if( NI && NI->hasControl() )
	{
		fugio::RenderInterface		*Render = qobject_cast<fugio::RenderInterface *>( NI->control()->qobject() );

		if( Render )
		{
			Render->render( NI->context()->global()->timestamp() );
		}
	}
}
