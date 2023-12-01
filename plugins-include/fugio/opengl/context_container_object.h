#ifndef CONTEXT_CONTAINER_OBJECT_H
#define CONTEXT_CONTAINER_OBJECT_H

#include <QObject>
#include <QSize>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLContext>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFunctions>

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

class ContextFramebufferObject
{
public:
	ContextFramebufferObject( void ) {}

	virtual ~ContextFramebufferObject( void )
	{
	}

	GLuint fbo( void )
	{
		QOpenGLContext	*Context = QOpenGLContext::currentContext();

		if( !Context )
		{
			return( 0 );
		}

		QOpenGLFunctions	*GL = Context->functions();

		FBOData FBO = mObjectMap.value( Context );

		if( !FBO.mFBO )
		{
			GL->glGenFramebuffers( 1, &FBO.mFBO );

			if( FBO.mFBO )
			{
				QObject::connect( Context, &QOpenGLContext::aboutToBeDestroyed, [=]( void )
				{
					FBOData FBO = mObjectMap.value( Context );

					if( FBO.mFBO )
					{
						QOpenGLFunctions	*GL = Context->functions();

						GL->glDeleteFramebuffers( 1, &FBO.mFBO );
					}

					mObjectMap.remove( Context );
				} );

				mObjectMap.insert( Context, FBO );
			}
		}

		return( FBO.mFBO );
	}

	void setSize( const QSize &pSize )
	{
		QOpenGLContext	*Context = QOpenGLContext::currentContext();

		if( !Context )
		{
			return;
		}

		QOpenGLFunctions	*GL = Context->functions();

		FBOData FBO = mObjectMap.value( Context );

		if( FBO.mFBO && FBO.mSize != pSize )
		{
			GL->glDeleteFramebuffers( 1, &FBO.mFBO );

			FBO.mFBO = 0;
		}

		FBO.mSize = pSize;

		mObjectMap.insert( Context, FBO );
	}

	GLuint fbo( void ) const
	{
		return( mObjectMap.value( QOpenGLContext::currentContext() ).mFBO );
	}

	GLuint fboConst( void ) const
	{
		return( mObjectMap.value( QOpenGLContext::currentContext() ).mFBO );
	}

	QSize size( void ) const
	{
		QOpenGLContext	*Context = QOpenGLContext::currentContext();

		if( !Context )
		{
			return( QSize() );
		}

		FBOData FBO = mObjectMap.value( Context );

		return( FBO.mSize );
	}

private:
	typedef struct FBOData
	{
		GLuint		mFBO;
		QSize		mSize;

		FBOData( void ) : mFBO( 0 ) {}

	} FBOData;

	QMap<QOpenGLContext *, FBOData>				 mObjectMap;
};

FUGIO_NAMESPACE_END

#endif // CONTEXT_CONTAINER_OBJECT_H
