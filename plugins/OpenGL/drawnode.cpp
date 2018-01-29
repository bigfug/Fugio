#include "drawnode.h"

#include <QMatrix4x4>
#include <QTime>

#if QT_VERSION >= QT_VERSION_CHECK( 5, 6, 0 )
#include <QOpenGLExtraFunctions>
#endif

#include <qmath.h>

#include <fugio/core/uuid.h>
#include <fugio/opengl/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/core/variant_interface.h>
#include <fugio/core/array_interface.h>
#include <fugio/opengl/buffer_interface.h>

#include "bufferpin.h"
#include "openglplugin.h"

QMap<QString,GLenum>					 DrawNode::mModeMap;

#define INSERT_MODE(x)		mModeMap.insert(#x,x)

DrawNode::DrawNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	if( mModeMap.isEmpty() )
	{
		INSERT_MODE( GL_POINTS );
		INSERT_MODE( GL_LINE_STRIP );
		INSERT_MODE( GL_LINE_LOOP );
		INSERT_MODE( GL_LINES );
		INSERT_MODE( GL_TRIANGLE_STRIP );
		INSERT_MODE( GL_TRIANGLE_FAN );
		INSERT_MODE( GL_TRIANGLES );
#if !defined( QT_OPENGL_ES_2 )
		INSERT_MODE( GL_LINE_STRIP_ADJACENCY );
		INSERT_MODE( GL_LINES_ADJACENCY );
		INSERT_MODE( GL_TRIANGLE_STRIP_ADJACENCY );
		INSERT_MODE( GL_TRIANGLES_ADJACENCY );
		INSERT_MODE( GL_PATCHES );
#endif
	}

	FUGID( PIN_MODE,			"A06B6399-6A5F-421E-8EBB-8A0E9A3EC861" );
	FUGID( PIN_INPUT_COUNT,		"EB9C6D35-F381-49F8-9772-EB1F9B14BB7B" );
	FUGID( PIN_INPUT_INSTANCES,	"6C5D0573-BE92-47CA-9AA4-E301DD56BD07" );
	FUGID( PIN_OUTPUT_RENDER,	"45FEFFDA-C883-4D4A-84FC-E0990C66C145" );
	FUGID( PIN_INPUT_INDEX,		"F6EAB673-9390-4DB8-8B41-E29D263F96E5" );

	mValMode = pinInput<fugio::ChoiceInterface *>( "Mode", mPinMode, PID_CHOICE, PIN_MODE );

	mValMode->setChoices( mModeMap.keys() );

	mPinMode->setValue( "GL_TRIANGLES" );

	mPinCount = pinInput( "Count", PIN_INPUT_COUNT );

	mPinInstances = pinInput( "Instances", PIN_INPUT_INSTANCES );

	mPinInputIndex = pinInput( "Index", PIN_INPUT_INDEX );

	mValOutputRender = pinOutput<fugio::RenderInterface *>( "Render", mPinOutputRender, PID_RENDER, PIN_OUTPUT_RENDER );

	mPinInstances->setValue( 0 );

	mPinCount->setValue( 1 );
}

bool DrawNode::initialise()
{
	if( !fugio::NodeControlBase::initialise() )
	{
		return( false );
	}

	if( !OpenGLPlugin::hasContextStatic() )
	{
		return( false );
	}

	return( true );
}

void DrawNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	pinUpdated( mPinOutputRender );
}

void DrawNode::render( qint64 pTimeStamp, QUuid pSourcePinId )
{
	Q_UNUSED( pTimeStamp )
	Q_UNUSED( pSourcePinId )

	if( !OpenGLPlugin::hasContextStatic() )
	{
		return;
	}

	initializeOpenGLFunctions();

	GLenum		Mode		= mModeMap.value( variant( mPinMode ).toString(), GL_TRIANGLES );
	int			Count		= variant( mPinCount ).toInt();
	int			Instances	= variant( mPinInstances ).toInt();

	if( Count <= 0 )
	{
		Count = std::numeric_limits<int>::max();
	}

#if defined( QOPENGLEXTRAFUNCTIONS_H )
	QOpenGLExtraFunctions	*GLEX = QOpenGLContext::currentContext()->extraFunctions();
#endif

	fugio::OpenGLBufferInterface	*B;

	if( ( B = input<fugio::OpenGLBufferInterface *>( mPinInputIndex ) ) != nullptr )
	{
		if( B->buffer() && B->buffer()->isCreated() && B->isIndex() )
		{
			Count = B->count();

			if( Count > 0 )
			{
				B->bind();

				if( Instances > 0 )
				{
#if defined( QOPENGLEXTRAFUNCTIONS_H )
					if( GLEX )
					{
						GLEX->glDrawElementsInstanced( Mode, Count, GL_UNSIGNED_INT, 0, Instances );
					}
#endif
				}
				else
				{
					glDrawElements( Mode, Count, GL_UNSIGNED_INT, 0 );
				}

				B->release();
			}
		}
	}
	else if( Instances > 0 )
	{
#if defined( QOPENGLEXTRAFUNCTIONS_H )
		if( GLEX )
		{
			GLEX->glDrawArraysInstanced( Mode, 0, Count, Instances );
		}
#endif
	}
	else if( Count > 0 && Count < std::numeric_limits<int>::max() )
	{
		glDrawArrays( Mode, 0, Count );
	}
}
