#ifndef BINDTEXTURENODE_H
#define BINDTEXTURENODE_H

#include "opengl_includes.h"

#include <QObject>

#include <fugio/nodecontrolbase.h>

#include <fugio/render_interface.h>
#include <fugio/opengl/node_render_interface.h>

class BindTextureNode : public fugio::NodeControlBase, public fugio::NodeRenderInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::NodeRenderInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Binds OpenGL textures" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Bind_Texture_OpenGL" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE BindTextureNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~BindTextureNode( void ) {}

	// NodeControlInterface interface
public:
	virtual bool initialise();

	virtual void inputsUpdated( qint64 pTimeStamp );

	// fugio::NodeRenderInterface interface
public:
	virtual void render( qint64 pTimeStamp, QUuid pSourcePinId );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinState;
	QSharedPointer<fugio::PinInterface>			 mPinGeometry;

	QSharedPointer<fugio::PinInterface>			 mPinOutput;
	fugio::RenderInterface						*mValOutput;

};

#endif // BINDTEXTURENODE_H
