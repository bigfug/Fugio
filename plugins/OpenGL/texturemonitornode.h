#ifndef TEXTUREMONITORNODE_H
#define TEXTUREMONITORNODE_H

//#include "opengl_includes.h"

#include <QObject>
#include <QDockWidget>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>

#include <fugio/nodecontrolbase.h>

#include "texturemonitor.h"

class TextureMonitorNode : public fugio::NodeControlBase, public QOpenGLFunctions
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", tr( "Provides a dockable window for monitoring OpenGL textures" ) )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "TextureMonitor" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE TextureMonitorNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~TextureMonitorNode( void ) {}

	// NodeControlInterface interface

	virtual bool initialise( void ) Q_DECL_OVERRIDE;

	virtual bool deinitialise( void ) Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual QList<QUuid> pinAddTypesInput() const Q_DECL_OVERRIDE;

	virtual bool canAcceptPin( fugio::PinInterface *pPin ) const Q_DECL_OVERRIDE;

	virtual bool pinShouldAutoRename(fugio::PinInterface *pPin) const Q_DECL_OVERRIDE;

	virtual void paintGL( void );

protected:
	QDockWidget					*mDockWidget;
	TextureMonitor				*mWidget;
	Qt::DockWidgetArea			 mDockArea;
	QOpenGLVertexArrayObject	 mVAO;
	QOpenGLShaderProgram		 mShader;

	GLuint						 mProgram;
	GLuint						 mVBO;
};

#endif // TEXTUREMONITORNODE_H
