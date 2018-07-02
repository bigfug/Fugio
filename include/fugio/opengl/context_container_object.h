#ifndef CONTEXT_CONTAINER_OBJECT_H
#define CONTEXT_CONTAINER_OBJECT_H

#include <QObject>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLContext>

#include <fugio/global.h>

FUGIO_NAMESPACE_BEGIN

class ContextVertexArrayObject
{
public:
	ContextVertexArrayObject( void ) {}

	virtual ~ContextVertexArrayObject( void )
	{
		for( QOpenGLVertexArrayObject *VAO : mObjectMap )
		{
			delete VAO;
		}

		mObjectMap.clear();
	}

	QOpenGLVertexArrayObject *vao( void )
	{
		QOpenGLContext	*Context = QOpenGLContext::currentContext();

		if( !Context )
		{
			return( Q_NULLPTR );
		}

		QOpenGLVertexArrayObject		*VAO = mObjectMap.value( Context );

		if( !VAO )
		{
			VAO = new QOpenGLVertexArrayObject();

			if( VAO )
			{
				QObject::connect( Context, &QOpenGLContext::aboutToBeDestroyed, [=]( void )
				{
					QOpenGLVertexArrayObject	*VAO = mObjectMap.value( Context );

					if( VAO )
					{
						delete VAO;
					}

					mObjectMap.remove( Context );

				} );

				mObjectMap.insert( Context, VAO );
			}
		}

		return( VAO );
	}

	QOpenGLVertexArrayObject *vao( void ) const
	{
		return( mObjectMap.value( QOpenGLContext::currentContext() ) );
	}

private:
	QMap<QOpenGLContext *, QOpenGLVertexArrayObject *>		mObjectMap;
};

FUGIO_NAMESPACE_END

#endif // CONTEXT_CONTAINER_OBJECT_H
