#ifndef TEXTUREMONITOR_H
#define TEXTUREMONITOR_H

//#include "opengl_includes.h"

#include <QObject>
#include <QOpenGLWidget>

class TextureMonitorNode;

class TextureMonitor : public QOpenGLWidget
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

protected:
	TextureMonitorNode		*mNode;
};

#endif // TEXTUREMONITOR_H
