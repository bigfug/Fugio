#ifndef NODE_RENDER_INTERFACE_H
#define NODE_RENDER_INTERFACE_H

#include <fugio/global.h>

FUGIO_NAMESPACE_BEGIN

class NodeRenderInterface
{
public:
	virtual ~NodeRenderInterface( void ) {}

	virtual void render( qint64 pTimeStamp, QUuid pSourcePinId ) = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::NodeRenderInterface, "com.bigfug.fugio.render.node/1.0" )

#endif // NODE_RENDER_INTERFACE_H
