#ifndef RENDER_INTERFACE_H
#define RENDER_INTERFACE_H

#include <fugio/global.h>

FUGIO_NAMESPACE_BEGIN

class RenderInterface
{
public:
	virtual ~RenderInterface( void ) {}

	virtual void render( qint64 pTimeStamp ) = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::RenderInterface, "com.bigfug.fugio.render/1.0" )

#endif // RENDER_INTERFACE_H
