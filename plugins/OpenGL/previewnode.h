#ifndef OPENGLPREVIEWNODE_H
#define OPENGLPREVIEWNODE_H

#include "opengl_includes.h"

#include <QDockWidget>

#include <fugio/opengl/uuid.h>
#include <fugio/nodecontrolbase.h>

#include <fugio/render_interface.h>

class Preview;

FUGIO_NAMESPACE_BEGIN
class OpenGLTextureInterface;
FUGIO_NAMESPACE_END

class PreviewNode : public fugio::NodeControlBase, public fugio::RenderInterface, public QOpenGLFunctions
{
	Q_OBJECT
	Q_INTERFACES( fugio::RenderInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "OpenGL Preview window" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Preview_(OpenGL)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE PreviewNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~PreviewNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual bool initialise( void ) Q_DECL_OVERRIDE;
	virtual bool deinitialise() Q_DECL_OVERRIDE;

	virtual void loadSettings( QSettings &pSettings ) Q_DECL_OVERRIDE;
	virtual void saveSettings( QSettings &pSettings ) const Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual QList<QUuid> pinAddTypesInput() const Q_DECL_OVERRIDE;

	virtual bool canAcceptPin( fugio::PinInterface *pPin ) const Q_DECL_OVERRIDE;

	//-------------------------------------------------------------------------
	// InterfaceOpenGLRenderer

	virtual void render( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

private:
	QDockWidget							*mDockWidget;
	Qt::DockWidgetArea					 mDockArea;
	Preview								*mOutput;

	QSharedPointer<fugio::PinInterface>	 mPinTexture;
	fugio::OpenGLTextureInterface				*mTexture;
};

#endif // OPENGLPREVIEWNODE_H
