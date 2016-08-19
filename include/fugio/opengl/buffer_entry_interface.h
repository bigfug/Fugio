#ifndef OPENGL_BUFFER_ENTRY_INTERFACE_H
#define OPENGL_BUFFER_ENTRY_INTERFACE_H

#include <fugio/global.h>

FUGIO_NAMESPACE_BEGIN

class OpenGLBufferEntryInterface
{
public:
	virtual ~OpenGLBufferEntryInterface( void ) {}

	virtual GLuint typeSize( void ) const = 0;

	virtual GLuint entrySize( void ) const = 0;

	virtual bool normalised( void ) const = 0;

	virtual void setSize( GLint pSize ) = 0;

	virtual void setType( GLenum pType ) = 0;

	virtual void setNormalised( GLboolean pNormalised ) = 0;

	virtual void bind( GLuint pIndex ) = 0;

	virtual void release( GLuint pIndex ) = 0;

	virtual const GLvoid *bind( GLuint pIndex, GLsizei pStride, const GLvoid *pPointer ) = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::OpenGLBufferEntryInterface, "com.bigfug.fugio.opengl.buffer-entry/1.0" )

#endif // OPENGL_BUFFER_ENTRY_INTERFACE_H
