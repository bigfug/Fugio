#ifndef CLEARNODE_H
#define CLEARNODE_H

#include "opengl_includes.h"

#include <QObject>

#include <fugio/nodecontrolbase.h>

#include <fugio/render_interface.h>
#include <fugio/opengl/node_render_interface.h>

class ClearNode : public fugio::NodeControlBase, public fugio::NodeRenderInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::NodeRenderInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "OpenGL Clear" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Clear_(OpenGL)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE ClearNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~ClearNode( void ) {}

	// NodeControlInterface interface
public:
	virtual bool initialise( void ) Q_DECL_OVERRIDE;

	// fugio::NodeRenderInterface interface
public:
	virtual void render( qint64 pTimeStamp, QUuid pSourcePinId ) Q_DECL_OVERRIDE;

protected:
	QSharedPointer<fugio::PinInterface>			 mPinOutput;
	fugio::RenderInterface						*mValOutput;
};

#endif // CLEARNODE_H
