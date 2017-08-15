#ifndef OPENGL_VERTEX_ARRAY_OBJECT_INTERFACE_H
#define OPENGL_VERTEX_ARRAY_OBJECT_INTERFACE_H

#include <fugio/global.h>

class QOpenGLVertexArrayObject;

FUGIO_NAMESPACE_BEGIN

class VertexArrayObjectInterface
{
public:
	virtual ~VertexArrayObjectInterface( void ) {}

	virtual QOpenGLVertexArrayObject &vao( void ) = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::VertexArrayObjectInterface, "com.bigfug.fugio.opengl.vao/1.0" )

#endif // OPENGL_VERTEX_ARRAY_OBJECT_INTERFACE_H
