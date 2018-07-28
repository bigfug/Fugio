#ifndef RENDERTOTEXTURENODE_H
#define RENDERTOTEXTURENODE_H

#include "opengl_includes.h"

#include <QObject>
#include <QSize>

#include <fugio/nodecontrolbase.h>

#include <fugio/render_interface.h>
#include <fugio/opengl/node_render_interface.h>
#include <fugio/opengl/context_container_object.h>

class RenderToTextureNode : public fugio::NodeControlBase, protected QOpenGLFunctions
{
	Q_OBJECT

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "OpenGL Render To Texture" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Render_To_Texture_(OpenGL)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE RenderToTextureNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~RenderToTextureNode( void ) {}

	// NodeControlInterface interface
public:
	virtual bool initialise( void ) Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputTrigger;
	QSharedPointer<fugio::PinInterface>			 mPinInputRender;

	QSharedPointer<fugio::PinInterface>			 mPinOutputTexture;

	fugio::ContextFramebufferObject				 mFBO;
};

#endif // RENDERTOTEXTURENODE_H
