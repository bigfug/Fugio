#include "transformfeedbacknode.h"

#include <QOpenGLFunctions_4_0_Core>

#if QT_VERSION >= QT_VERSION_CHECK( 5, 6, 0 )
#include <QOpenGLExtraFunctions>
#endif

#include "openglplugin.h"

#include <fugio/opengl/buffer_entry_interface.h>
#include <fugio/node_signals.h>

#include "openglplugin.h"

QMap<QString,GLenum>		 TransformFeedbackNode::mModeMap;

#define INSERT_MODE(x)		mModeMap.insert(#x,x)

TransformFeedbackNode::TransformFeedbackNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PID_INPUT_TYPE,	"a853352f-7c6d-4393-8d36-d86a912169da" );
	FUGID( PID_INPUT_COUNT,	"829e5544-0f99-470b-9b37-37e3d78cf236" );
	FUGID( PID_INPUT_SIZE,	"CE182B9E-045C-403D-9D90-E0AAAFAF854B" );
	FUGID( PID_BUFFER0,		"3f2bc100-e572-43a3-9f61-0d1c661e28bd" );
	FUGID( PID_RENDER1,		"7ddf9026-1fcc-4ca1-9bd0-94396e124149" );
	FUGID( PID_RENDER2,		"6AF063E1-1FE4-4D91-8857-A9945EE12E8A" );

	if( mModeMap.isEmpty() )
	{
		INSERT_MODE( GL_POINTS );
		INSERT_MODE( GL_LINE_STRIP );
		INSERT_MODE( GL_LINE_LOOP );
		INSERT_MODE( GL_LINES );
		INSERT_MODE( GL_TRIANGLE_STRIP );
		INSERT_MODE( GL_TRIANGLE_FAN );
		INSERT_MODE( GL_TRIANGLES );
#if !defined( GL_ES_VERSION_2_0 )
		INSERT_MODE( GL_LINE_STRIP_ADJACENCY );
		INSERT_MODE( GL_LINES_ADJACENCY );
		INSERT_MODE( GL_TRIANGLE_STRIP_ADJACENCY );
		INSERT_MODE( GL_TRIANGLES_ADJACENCY );
		INSERT_MODE( GL_PATCHES );
#endif
	}

	mValInputType = pinInput<fugio::ChoiceInterface *>( "Type", mPinInputType, PID_CHOICE, PID_INPUT_TYPE );

	mValInputType->setChoices( mModeMap.keys() );

	mPinInputType->setValue( "GL_POINTS" );

	mPinInputCount = pinInput( "Count", PID_INPUT_COUNT );

	mValGeometry1 = pinOutput<fugio::RenderInterface *>( "Transform", mPinGeometry1, PID_RENDER, PID_RENDER1 );

	mValGeometry2 = pinOutput<fugio::RenderInterface *>( "Feedback", mPinGeometry2, PID_RENDER, PID_RENDER2 );

	QSharedPointer<fugio::PinInterface>				 PinOutputBuffer0;
	fugio::OpenGLBufferInterface					*ValOutputBuffer0;

	QSharedPointer<fugio::PinInterface>				 PinInputSize0 = pinInput( "Size", PID_INPUT_SIZE );

	ValOutputBuffer0 = pinOutput<fugio::OpenGLBufferInterface *>( "Buffer", PinOutputBuffer0, PID_OPENGL_BUFFER, PID_BUFFER0 );

	ValOutputBuffer0->setDoubleBuffered( true );

	mNode->pairPins( PinInputSize0, PinOutputBuffer0 );

	mTrnFbkId[ 0 ] = 0;
	mTrnFbkId[ 1 ] = 0;

	mBufferCount = 0;

	mTransformMode = GL_POINTS;

	mFirstTransform = true;

	m_currTFB = 0;
}

TransformFeedbackNode::~TransformFeedbackNode()
{
}

bool TransformFeedbackNode::initialise()
{
	if( !fugio::NodeControlBase::initialise() )
	{
		return( false );
	}

	if( !OpenGLPlugin::hasContextStatic() )
	{
		return( false );
	}

#if defined( QOPENGLEXTRAFUNCTIONS_H )
	QOpenGLExtraFunctions	*GLEX = QOpenGLContext::currentContext()->extraFunctions();

	if( GLEX )
	{
		GLEX->glGenTransformFeedbacks( 2, mTrnFbkId );
	}
	else
#endif
	{
		return( false );
	}

	return( true );
}

bool TransformFeedbackNode::deinitialise()
{
#if defined( QOPENGLEXTRAFUNCTIONS_H )
	QOpenGLExtraFunctions	*GLEX = QOpenGLContext::currentContext()->extraFunctions();

	if( GLEX )
	{
		GLEX->glDeleteTransformFeedbacks( 2, mTrnFbkId );
	}
#endif

	return( NodeControlBase::deinitialise() );
}

void TransformFeedbackNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	initializeOpenGLFunctions();

#if defined( QOPENGLEXTRAFUNCTIONS_H )
	QOpenGLExtraFunctions	*GLEX = QOpenGLContext::currentContext()->extraFunctions();
#endif

	// The transform mode is a sub-set of the draw modes

	mDrawMode = mModeMap.value( variant( mPinInputType ).toString() );

	switch( mDrawMode )
	{
		case GL_LINE_LOOP:
		case GL_LINE_STRIP:
#if !defined( GL_ES_VERSION_2_0 )
		case GL_LINES_ADJACENCY:
		case GL_LINE_STRIP_ADJACENCY:
#endif
			mTransformMode = GL_LINES;
			break;

		case GL_TRIANGLE_STRIP:
		case GL_TRIANGLE_FAN:
#if !defined( GL_ES_VERSION_2_0 )
		case GL_TRIANGLES_ADJACENCY:
		case GL_TRIANGLE_STRIP_ADJACENCY:
#endif
			mTransformMode = GL_TRIANGLES;
			break;

		default:
			mTransformMode = mDrawMode;
			break;
	}

	// Get the count of entries in the buffer

	int			BufferCount = variant( mPinInputCount ).toInt();

	mBufferCount = qMax( 0, BufferCount );

	//

	int		XFIndex = 0;

	for( fugio::NodeInterface::UuidPair UP : mNode->pairedPins() )
	{
		QSharedPointer<fugio::PinInterface>		PinI, PinO;

		PinI = mNode->findPinByLocalId( UP.first );
		PinO = mNode->findPinByLocalId( UP.second );

		if( !PinI || !PinO || !PinO->hasControl() )
		{
			continue;
		}

		int			EntSiz = variant( PinI ).toInt();

		if( EntSiz <= 0 )
		{
			continue;
		}

		fugio::OpenGLBufferInterface *BufO = qobject_cast<fugio::OpenGLBufferInterface *>( PinO->control()->qobject() );

		if( !BufO )
		{
			continue;
		}

		// reallocate the buffer if needed

		if( BufO->stride() != EntSiz || BufO->count() != mBufferCount )
		{
			// Allocate a new buffer for our data

			if( !BufO->alloc( QMetaType::Float, EntSiz, EntSiz, mBufferCount ) )
			{
				mNode->setStatus( fugio::NodeInterface::Error );

				return;
			}

#if defined( QOPENGLEXTRAFUNCTIONS_H )
			if( GLEX )
			{
				// Set up the first TF destination buffer

				GLEX->glBindTransformFeedback( GL_TRANSFORM_FEEDBACK, mTrnFbkId[ 0 ] );

				GLEX->glBindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, XFIndex, BufO->dstBuf()->bufferId() );

				// Set up the second TF destination buffer

				GLEX->glBindTransformFeedback( GL_TRANSFORM_FEEDBACK, mTrnFbkId[ 1 ] );

				GLEX->glBindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, XFIndex, BufO->srcBuf()->bufferId() );

				// Reset the TF bind

				GLEX->glBindTransformFeedback( GL_TRANSFORM_FEEDBACK, 0 );
			}
#endif

			// We'll use the first TF buffer, with dstBuf() as our target

			m_currTFB = 0;

			mFirstTransform = true;

			pinUpdated( PinO );
		}

		XFIndex++;
	}

	mNode->setStatus( mBufferCount > 0 ? fugio::NodeInterface::Initialised : fugio::NodeInterface::Warning );

	OPENGL_DEBUG( mNode->name() );
}

void TransformFeedbackNode::render( qint64 pTimeStamp, QUuid pSourcePinId )
{
	Q_UNUSED( pTimeStamp )

	if( mNode->status() != fugio::NodeInterface::Initialised )
	{
		return;
	}

	if( pSourcePinId == mPinGeometry1->localId() )
	{
		render1( pTimeStamp );
	}
	else if( pSourcePinId == mPinGeometry2->localId() )
	{
		render2( pTimeStamp );
	}

	OPENGL_DEBUG( mNode->name() );
}

//#define TF_QUERY

void TransformFeedbackNode::render1( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	initializeOpenGLFunctions();

#if defined( QOPENGLEXTRAFUNCTIONS_H )
	QOpenGLExtraFunctions	*GLEX = QOpenGLContext::currentContext()->extraFunctions();
#endif

#if !defined( QT_OPENGL_ES_2 )
	QOpenGLFunctions_4_0_Core	*GL40 = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_0_Core>();

	if( GL40 && !GL40->initializeOpenGLFunctions() )
	{
		GL40 = Q_NULLPTR;
	}
#endif

	glEnable( GL_RASTERIZER_DISCARD );

#if defined( TF_QUERY )
	GLuint query;

	glGenQueries( 1, &query );

	glBeginQuery( GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, query );
#endif

#if defined( QOPENGLEXTRAFUNCTIONS_H )
	if( GLEX )
	{
		GLEX->glBindTransformFeedback( GL_TRANSFORM_FEEDBACK, mTrnFbkId[ currTFB() ] );

		// Draw the buffers (using the previous buffer as the primitive count if !mFirstTransform)

		GLEX->glBeginTransformFeedback( mTransformMode );

		if( mFirstTransform )
		{
			glDrawArrays( mDrawMode, 0, qMin( 1, mBufferCount ) );

			mFirstTransform = false;
		}
#if !defined( QT_OPENGL_ES_2 )
		else if( GL40 )
		{
			GL40->glDrawTransformFeedback( mDrawMode, mTrnFbkId[ nextTFB() ] );
		}
#endif

		GLEX->glEndTransformFeedback();

		// Clear our TF binding and turn off GL_RASTERIZER_DISCARD

		GLEX->glBindTransformFeedback( GL_TRANSFORM_FEEDBACK, 0 );
	}
#endif

#if defined( TF_QUERY )
	glEndQuery( GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN );

	GLuint primitives;
	static GLuint last = 0;
	glGetQueryObjectuiv(query, GL_QUERY_RESULT, &primitives);

	if( last != primitives )
	{
		qDebug() << primitives;

		last = primitives;
	}

	glDeleteQueries( 1, &query );
#endif

	glDisable( GL_RASTERIZER_DISCARD );

	// Increment the TF buffer index so we'll use the next one for the next call

	m_currTFB = nextTFB();

	for( QSharedPointer<fugio::PinInterface> P : mNode->enumOutputPins() )
	{
		if( !P->hasControl() )
		{
			continue;
		}

		fugio::OpenGLBufferInterface *BufO = qobject_cast<fugio::OpenGLBufferInterface *>( P->control()->qobject() );

		if( !BufO )
		{
			continue;
		}

		// Also swap the output buffer

		BufO->swapBuffers();

		// Mark the output buffer as updated

		pinUpdated( P );
	}

	pinUpdated( mPinGeometry2 );

	OPENGL_DEBUG( mNode->name() );
}

void TransformFeedbackNode::render2( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	if( !mFirstTransform )
	{
#if !defined( QT_OPENGL_ES_2 )
		QOpenGLFunctions_4_0_Core	*GL40 = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_0_Core>();

		if( GL40 && GL40->initializeOpenGLFunctions() )
		{
			GL40->glDrawTransformFeedback( mDrawMode, mTrnFbkId[ nextTFB() ] );
		}
#endif
	}

	OPENGL_DEBUG( mNode->name() );
}

QList<QUuid> TransformFeedbackNode::pinAddTypesInput() const
{
	static QList<QUuid>		PinLst =
	{
		PID_INTEGER
	};

	return( PinLst );
}

bool TransformFeedbackNode::canAcceptPin( PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_OUTPUT );
}

QUuid TransformFeedbackNode::pairedPinControlUuid( QSharedPointer<PinInterface> pPin ) const
{
	if( pPin->direction() == PIN_INPUT )
	{
		return( PID_OPENGL_BUFFER );
	}

	return( QUuid() );
}
