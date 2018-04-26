#ifndef CONTEXTNODE_H
#define CONTEXTNODE_H

#include "opengl_includes.h"

#include <QOffscreenSurface>
#include <QOpenGLDebugLogger>

#include <fugio/nodecontrolbase.h>

class ContextNode : public fugio::NodeControlBase
{
	Q_OBJECT

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Creates an OpenGL context" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Context_(OpenGL)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit ContextNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~ContextNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual bool initialise( void ) Q_DECL_OVERRIDE;

	virtual bool deinitialise( void ) Q_DECL_OVERRIDE;

private slots:
	void handleLoggedMessage( const QOpenGLDebugMessage &debugMessage )
	{
		qDebug() << mNode->name() << debugMessage;
	}

private:
	QOffscreenSurface					 mSurface;
	QOpenGLContext						 mContext;
	QOpenGLDebugLogger					 mDebugLogger;
};


#endif // CONTEXTNODE_H
