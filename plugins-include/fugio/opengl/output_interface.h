#ifndef OPENGL_OUTPUT_INTERFACE_H
#define OPENGL_OUTPUT_INTERFACE_H

#include <fugio/global.h>

class OpenGLOutputInterface
{
public:
	virtual ~OpenGLOutputInterface( void ) {}

//	virtual void registerCursorMove( QObject *pObject, const char *pMember ) = 0;
};

Q_DECLARE_INTERFACE( OpenGLOutputInterface, "com.bigfug.fugio.opengl.output/1.0" )

#endif // OPENGL_OUTPUT_INTERFACE_H
