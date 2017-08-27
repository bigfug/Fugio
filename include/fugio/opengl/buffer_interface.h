#ifndef OPENGL_BUFFER_INTERFACE_H
#define OPENGL_BUFFER_INTERFACE_H

#include <fugio/global.h>

#include <QOpenGLBuffer>

FUGIO_NAMESPACE_BEGIN

class OpenGLBufferInterface
{
public:
	virtual ~OpenGLBufferInterface( void ) {}

	virtual bool bind( void ) = 0;

	virtual void release( void ) = 0;

	virtual bool alloc( QMetaType::Type pType, int pSize, int pStride, int pCount, const void *pData = 0 ) = 0;

	virtual void clear( void ) = 0;

	virtual const QOpenGLBuffer *buffer( void ) const = 0;

	virtual QOpenGLBuffer *buffer( void ) = 0;

	virtual QOpenGLBuffer::Type target( void ) const = 0;

	virtual int count( void ) const = 0;

	virtual QMetaType::Type type( void ) const = 0;

	virtual int stride( void ) const = 0;

	virtual int size( void ) const = 0;

	virtual bool instanced( void ) const = 0;

	virtual void setIndex( bool pIndex ) = 0;

	virtual void setInstanced( bool pInstanced ) = 0;

	virtual void setTarget( QOpenGLBuffer::Type pTarget ) = 0;

	virtual void setDoubleBuffered( bool pDoubleBuffered ) = 0;

	virtual bool isDoubleBuffered( void ) const = 0;

	virtual bool isIndex( void ) const = 0;

	virtual void swapBuffers( void ) = 0;

	virtual const QOpenGLBuffer *srcBuf( void ) const = 0;

	virtual const QOpenGLBuffer *dstBuf( void ) const = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::OpenGLBufferInterface, "com.bigfug.fugio.opengl.buffer/1.0" )

#endif // OPENGL_BUFFER_INTERFACE_H
