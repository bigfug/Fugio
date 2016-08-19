#include "vertexarrayobjectnode.h"

#include "openglplugin.h"

#include <fugio/opengl/shader_interface.h>
#include <fugio/opengl/buffer_entry_interface.h>
#include <fugio/opengl/buffer_interface.h>

VertexArrayObjectNode::VertexArrayObjectNode( QSharedPointer<fugio::NodeInterface> pNode )
	:NodeControlBase( pNode ), mVAO( 0 )
{
	FUGID( PIN_INPUT_SHADER,	"17e082db-5757-4e65-8899-cf38fb6fae23" );
	FUGID( PIN_OUTPUT_VAO,		"71e23f1a-273e-471c-b538-4c050829ef39" );

	mPinInputShader = pinInput( "Shader", PIN_INPUT_SHADER );

	mPinInputShader->registerPinInputType( PID_OPENGL_SHADER );

	mValOutputVAO = pinOutput<VertexArrayObjectPin *>( "VAO", mPinOutputVAO, PID_OPENGL_VERTEX_ARRAY_OBJECT, PIN_OUTPUT_VAO );
}

bool VertexArrayObjectNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	if( !OpenGLPlugin::hasContextStatic() )
	{
		return( false );
	}

	return( true );
}

void VertexArrayObjectNode::freeVAO()
{
#if !defined( Q_OS_RASPBERRY_PI )
	if( mVAO )
	{
		glDeleteVertexArrays( 1, &mVAO );

		mVAO = 0;
	}
#endif

	mValOutputVAO->setVAO( mVAO );

	mBindInfo.clear();
}

bool VertexArrayObjectNode::deinitialise()
{
	freeVAO();

	return( NodeControlBase::deinitialise() );
}

void VertexArrayObjectNode::inputsUpdated( qint64 pTimeStamp )
{
	OpenGLShaderInterface		*Shader = input<OpenGLShaderInterface *>( mPinInputShader );

	if( Shader )
	{
		if( mPinInputShader->isUpdated( pTimeStamp ) )
		{
			freeVAO();
		}
	}

	bool			UpdateAll = false;

	if( !mVAO )
	{
#if !defined( Q_OS_RASPBERRY_PI )
		glGenVertexArrays( 1, &mVAO );
		mValOutputVAO->setVAO( mVAO );

		if( !mVAO )
		{
			return;
		}
#endif

		UpdateAll = true;
	}

	for( QSharedPointer<fugio::PinInterface> P : mNode->enumInputPins() )
	{
		if( P == mPinInputShader )
		{
			continue;
		}

		if( !UpdateAll && !P->isUpdated( pTimeStamp ) )
		{
			continue;
		}

		ShaderUniformMap::const_iterator it = Shader->attributeMap().find( P->name() );

		if( it == Shader->attributeMap().end() )
		{
			continue;
		}

		fugio::PinControlInterface	*PinControl = input<fugio::PinControlInterface *>( P );

		if( !PinControl )
		{
			continue;
		}

		fugio::OpenGLBufferInterface		*Buffer = qobject_cast<fugio::OpenGLBufferInterface *>( PinControl->qobject() );

		const ShaderUniformData				&UniformData = it.value();

		bool			UpdateBind = UpdateAll;

		if( !UpdateBind )
		{
			BindInfoMap::const_iterator		bi = mBindInfo.find( P->name() );

			if( bi == mBindInfo.end() )
			{
				UpdateBind = true;
			}
			else
			{
				const BindInfo &BI = bi.value();

				if( UniformData.mLocation != BI.mLocation )
				{
					UpdateBind = true;
				}

				if( Buffer && Buffer->type() != BI.mMetaType )
				{
					UpdateBind = true;
				}
			}
		}

		if( !UpdateBind )
		{
			continue;
		}

#if !defined( Q_OS_RASPBERRY_PI )
		glBindVertexArray( mVAO );
#endif

		if( true )
		{
			BindInfo		NewBI;

			NewBI.mLocation = UniformData.mLocation;
			NewBI.mMetaType     = ( Buffer ? Buffer->type() : QMetaType::UnknownType );

			mBindInfo.insert( P->name(), NewBI );
		}

		fugio::OpenGLBufferEntryInterface	*BufEnt = qobject_cast<fugio::OpenGLBufferEntryInterface *>( PinControl->qobject() );

		if( BufEnt )
		{
			BufEnt->bind( UniformData.mLocation );

			continue;
		}

		if( Buffer )
		{
			switch( Buffer->type() )
			{
				case QMetaType::Int:
					if( Buffer->bind() )
					{
#if !defined( Q_OS_RASPBERRY_PI )
						if( GLEW_ARB_vertex_array_object )
						{
							glVertexAttribIPointer( UniformData.mLocation, 1, GL_INT, 0, BUFFER_OFFSET( 0 ) );

							glEnableVertexAttribArray( UniformData.mLocation );
						}
						else
#endif
						{

						}

#if defined( glVertexAttribDivisor )
						if( Buffer->instanced() && GLEW_ARB_instanced_arrays )
						{
							glVertexAttribDivisor( UniformData.mLocation, 1 );
						}
#endif
					}
					break;

				case QMetaType::Float:
					if( Buffer->bind() )
					{
#if !defined( Q_OS_RASPBERRY_PI )
						if( GLEW_ARB_vertex_array_object )
						{
							glVertexAttribPointer( UniformData.mLocation, 1, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET( 0 ) );

							glEnableVertexAttribArray( UniformData.mLocation );
						}
						else
#endif
						{

						}

#if defined( glVertexAttribDivisor )
						if( Buffer->instanced() && GLEW_ARB_instanced_arrays )
						{
							glVertexAttribDivisor( UniformData.mLocation, 1 );
						}
#endif
					}
					break;

				case QMetaType::QPoint:
					if( Buffer->bind() )
					{
#if !defined( Q_OS_RASPBERRY_PI )
						if( GLEW_ARB_vertex_array_object )
						{
							glVertexAttribIPointer( UniformData.mLocation, 2, GL_INT, 0, BUFFER_OFFSET( 0 ) );

							glEnableVertexAttribArray( UniformData.mLocation );
						}
						else
#endif
						{
							//glVertexAttribPointer( UniformData.mLocation, 2, GL_FLOAT, GL_FALSE, 0, Buffer-> );
						}

#if defined( glVertexAttribDivisor )
						if( Buffer->instanced() && GLEW_ARB_instanced_arrays )
						{
							glVertexAttribDivisor( UniformData.mLocation, 1 );
						}
#endif
					}
					break;

				case QMetaType::QVector2D:
				case QMetaType::QPointF:
					if( Buffer->bind() )
					{
#if !defined( Q_OS_RASPBERRY_PI )
						if( GLEW_ARB_vertex_array_object )
						{
							glVertexAttribPointer( UniformData.mLocation, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET( 0 ) );

							glEnableVertexAttribArray( UniformData.mLocation );
						}
						else
#endif
						{
							//glVertexAttribPointer( UniformData.mLocation, 2, GL_FLOAT, GL_FALSE, 0, Buffer-> );
						}

#if defined( glVertexAttribDivisor )
						if( Buffer->instanced() && GLEW_ARB_instanced_arrays )
						{
							glVertexAttribDivisor( UniformData.mLocation, 1 );
						}
#endif
					}
					break;

				case QMetaType::QVector3D:
					if( Buffer->bind() )
					{
#if !defined( Q_OS_RASPBERRY_PI )
						if( GLEW_ARB_vertex_array_object )
						{
							OPENGL_DEBUG( mNode->name() );

							glVertexAttribPointer( UniformData.mLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET( 0 ) );

							OPENGL_DEBUG( mNode->name() );

							glEnableVertexAttribArray( UniformData.mLocation );

							OPENGL_DEBUG( mNode->name() );
						}
						else
#endif
						{

						}

#if defined( glVertexAttribDivisor )
						if( Buffer->instanced() && GLEW_ARB_instanced_arrays )
						{
							glVertexAttribDivisor( UniformData.mLocation, 1 );
						}
#endif
						OPENGL_DEBUG( mNode->name() );
					}
					break;

				case QMetaType::QVector4D:
					if( Buffer->bind() )
					{
#if !defined( Q_OS_RASPBERRY_PI )
						if( GLEW_ARB_vertex_array_object )
						{
							glVertexAttribPointer( UniformData.mLocation, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET( 0 ) );

							glEnableVertexAttribArray( UniformData.mLocation );
						}
						else
#endif
						{

						}

#if defined( glVertexAttribDivisor )
						if( Buffer->instanced() && GLEW_ARB_instanced_arrays )
						{
							glVertexAttribDivisor( UniformData.mLocation, 1 );
						}
#endif
					}
					break;

				case QMetaType::QMatrix4x4:
					if( Buffer->bind() )
					{
						for( int i = 0 ; i < 4 ; i++ )
						{
#if !defined( Q_OS_RASPBERRY_PI )
							if( GLEW_ARB_vertex_array_object )
							{
								glVertexAttribPointer( UniformData.mLocation + i, 4, GL_FLOAT, GL_FALSE, sizeof( GLfloat ) * 4 * 4, BUFFER_OFFSET( sizeof( GLfloat ) * 4 * i ) );

								glEnableVertexAttribArray( UniformData.mLocation + i );
							}
							else
#endif
							{

							}

#if defined( glVertexAttribDivisor )
							if( Buffer->instanced() && GLEW_ARB_instanced_arrays )
							{
								glVertexAttribDivisor( UniformData.mLocation + i, 1 );
							}
#endif
						}
					}
					break;

				default:
					//qWarning() << "Unknown meta type for VAO:" << QString( QMetaType::typeName( Buffer->type() ) ) << QString::number( Buffer->type() );
					break;
			}

#if defined( glVertexAttribDivisor )
			if( !Buffer->instanced() && GLEW_ARB_instanced_arrays )
			{
				glVertexAttribDivisor( UniformData.mLocation, 0 );
			}
#endif
		}
	}

#if !defined( Q_OS_RASPBERRY_PI )
	glBindVertexArray( 0 );
#endif

	pinUpdated( mPinOutputVAO );
}

QList<QUuid> VertexArrayObjectNode::pinAddTypesInput() const
{
	static QList<QUuid> PinLst =
	{
		PID_OPENGL_BUFFER,
		PID_OPENGL_BUFFER_ENTRY,
		PID_OPENGL_BUFFER_ENTRY_PROXY
	};

	return( PinLst );
}

bool VertexArrayObjectNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_OUTPUT );
}


QStringList VertexArrayObjectNode::availableInputPins() const
{
	QStringList					 PinLst;

	OpenGLShaderInterface		*Shader = input<OpenGLShaderInterface *>( mPinInputShader );

	if( Shader )
	{
		PinLst = Shader->attributeMap().keys();
	}

	for( QSharedPointer<fugio::PinInterface> P : mNode->enumInputPins() )
	{
		PinLst.removeAll( P->name() );
	}

	qSort( PinLst );

	return( PinLst );
}
