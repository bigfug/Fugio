#ifndef TEXTUREMONITOR_H
#define TEXTUREMONITOR_H

#include "opengl_includes.h"

#include <QObject>
#include <QOpenGLWidget>
#include <QOpenGLDebugLogger>

class TextureMonitorNode;

class TextureMonitor : public QOpenGLWidget, private QOpenGLFunctions
{
	Q_OBJECT

public:
	TextureMonitor();

	virtual ~TextureMonitor( void ) {}

	void setNode( TextureMonitorNode *pNode );

	// QOpenGLWidget interface
protected:
	virtual void initializeGL() Q_DECL_OVERRIDE;
	virtual void resizeGL(int w, int h) Q_DECL_OVERRIDE;
	virtual void paintGL() Q_DECL_OVERRIDE;

private slots:
	void handleLoggedMessage( const QOpenGLDebugMessage &debugMessage );

protected:
	TextureMonitorNode			*mNode;
	QOpenGLDebugLogger			 mDebugLogger;
};

#endif // TEXTUREMONITOR_H
