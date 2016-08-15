#ifndef PREVIEW_H
#define PREVIEW_H

#include "opengl_includes.h"

#include <QWindow>

#include <fugio/node_interface.h>

class PreviewNode;

class Preview : public QWindow
{
	Q_OBJECT

public:
	explicit Preview( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~Preview( void );

#if 0
	bool renderInit();
	void renderStart();
	void renderEnd();
	void renderLater();
	void initialize();
	void renderNow( void );
signals:

public slots:

protected:
	void exposeEvent(QExposeEvent *pEvent);
	void resizeEvent(QResizeEvent *pEvent);
	bool event(QEvent *event);

	void signalUpdate();

private:
	QWeakPointer<fugio::NodeInterface>			 mNode;
	QOpenGLContext						*mContext;
//	QOpenGLDebugLogger					*mLogger;
	bool								 m_update_pending;
	bool								 mNeedsInitialize;
#endif

	// QWindow interface
protected:
	virtual void resizeEvent(QResizeEvent *) Q_DECL_OVERRIDE;
	virtual void moveEvent(QMoveEvent *) Q_DECL_OVERRIDE;
	virtual void exposeEvent(QExposeEvent *) Q_DECL_OVERRIDE;
	virtual void showEvent(QShowEvent *) Q_DECL_OVERRIDE;
	virtual void hideEvent(QHideEvent *) Q_DECL_OVERRIDE;
};

#endif // PREVIEW_H
