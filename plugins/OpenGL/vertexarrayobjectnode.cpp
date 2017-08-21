#include "vertexarrayobjectnode.h"

#include <QOpenGLFunctions_3_0>
#include <QOpenGLFunctions_3_3_Core>

#include "openglplugin.h"

#include <fugio/opengl/shader_interface.h>
#include <fugio/opengl/buffer_entry_interface.h>
#include <fugio/opengl/buffer_interface.h>

VertexArrayObjectNode::VertexArrayObjectNode( QSharedPointer<fugio::NodeInterface> pNode )
	:NodeControlBase( pNode )
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
	mVAO.destroy();

	mBindInfo.clear();
}

bool VertexArrayObjectNode::deinitialise()
{
	freeVAO();

	return( NodeControlBase::deinitialise() );
}

void VertexArrayObjectNode::inputsUpdated( qint64 pTimeStamp )
{
	if( !pTimeStamp )
	{
		return;
	}

	bool			UpdateAll = false;

	if( !QOpenGLContext::currentContext() )
	{
		return;
	}

	if( !mVAO.isCreated() )
	{
		mVAO.create();

		UpdateAll = true;
	}

	if( mPinInputShader->isUpdated( pTimeStamp ) )
	{
		UpdateAll = true;
	}

	OpenGLShaderInterface		*Shader = input<OpenGLShaderInterface *>( mPinInputShader );

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

		mVAO.bind();

		bindPin( P.data(), PinControl, Buffer, UniformData );
	}

	mVAO.release();

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


void VertexArrayObjectNode::vaoBind()
{
	bool			UpdateAll = true;

	if( !QOpenGLContext::currentContext() )
	{
		return;
	}

	if( !mVAO.isCreated() )
	{
		mVAO.create();

		UpdateAll = true;
	}

	if( mVAO.isCreated() )
	{
		mVAO.bind();
	}

	if( UpdateAll )
	{
		OpenGLShaderInterface		*Shader = input<OpenGLShaderInterface *>( mPinInputShader );

		for( QSharedPointer<fugio::PinInterface> P : mNode->enumInputPins() )
		{
			if( P == mPinInputShader )
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

			bindPin( P.data(), PinControl, Buffer, UniformData );
		}
	}
}

void VertexArrayObjectNode::vaoRelease()
{
	mVAO.release();
}

void VertexArrayObjectNode::bindPin( fugio::PinInterface *P, fugio::PinControlInterface *PinControl , fugio::OpenGLBufferInterface *Buffer , const ShaderUniformData &UniformData)
{
	QOpenGLFunctions_3_0		*GL30 = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_0>();

	if( GL30 && !GL30->initializeOpenGLFunctions() )
	{
		GL30 = Q_NULLPTR;
	}

	QOpenGLFunctions_3_3_Core	*GL33 = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();

	if( GL33 && !GL33->initializeOpenGLFunctions() )
	{
		GL33 = Q_NULLPTR;
	}

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

		return;
	}

	if( Buffer )
	{
		switch( Buffer->type() )
		{
			case QMetaType::Int:
				if( Buffer->bind() )
				{
					if( GL30 )
					{
						GL30->glVertexAttribIPointer( UniformData.mLocation, 1, GL_INT, 0, BUFFER_OFFSET( 0 ) );

						glEnableVertexAttribArray( UniformData.mLocation );
					}

					if( Buffer->instanced() && GL33 )
					{
						GL33->glVertexAttribDivisor( UniformData.mLocation, 1 );
					}
				}
				break;

			case QMetaType::Float:
				if( Buffer->bind() )
				{
					glVertexAttribPointer( UniformData.mLocation, 1, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET( 0 ) );

					glEnableVertexAttribArray( UniformData.mLocation );

					if( Buffer->instanced() && GL33 )
					{
						GL33->glVertexAttribDivisor( UniformData.mLocation, 1 );
					}
				}
				break;

			case QMetaType::QPoint:
				if( Buffer->bind() )
				{
					if( GL30 )
					{
						GL30->glVertexAttribIPointer( UniformData.mLocation, 2, GL_INT, 0, BUFFER_OFFSET( 0 ) );

						glEnableVertexAttribArray( UniformData.mLocation );
					}
					else
					{
						//glVertexAttribPointer( UniformData.mLocation, 2, GL_FLOAT, GL_FALSE, 0, Buffer-> );
					}

					if( Buffer->instanced() && GL33 )
					{
						GL33->glVertexAttribDivisor( UniformData.mLocation, 1 );
					}
				}
				break;

			case QMetaType::QVector2D:
			case QMetaType::QPointF:
				if( Buffer->bind() )
				{
					if( GL30 )
					{
						glVertexAttribPointer( UniformData.mLocation, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET( 0 ) );

						glEnableVertexAttribArray( UniformData.mLocation );
					}
					else
					{
						//glVertexAttribPointer( UniformData.mLocation, 2, GL_FLOAT, GL_FALSE, 0, Buffer-> );
					}

					if( Buffer->instanced() && GL33 )
					{
						GL33->glVertexAttribDivisor( UniformData.mLocation, 1 );
					}
				}
				break;

			case QMetaType::QVector3D:
				if( Buffer->bind() )
				{
					glVertexAttribPointer( UniformData.mLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET( 0 ) );

					glEnableVertexAttribArray( UniformData.mLocation );

					if( Buffer->instanced() && GL33 )
					{
						GL33->glVertexAttribDivisor( UniformData.mLocation, 1 );
					}

					OPENGL_DEBUG( mNode->name() );
				}
				break;

			case QMetaType::QVector4D:
				if( Buffer->bind() )
				{
					glVertexAttribPointer( UniformData.mLocation, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET( 0 ) );

					glEnableVertexAttribArray( UniformData.mLocation );

					if( Buffer->instanced() && GL33 )
					{
						GL33->glVertexAttribDivisor( UniformData.mLocation, 1 );
					}
				}
				break;

			case QMetaType::QMatrix4x4:
				if( Buffer->bind() )
				{
					for( int i = 0 ; i < 4 ; i++ )
					{
						glVertexAttribPointer( UniformData.mLocation + i, 4, GL_FLOAT, GL_FALSE, sizeof( GLfloat ) * 4 * 4, BUFFER_OFFSET( sizeof( GLfloat ) * 4 * i ) );

						glEnableVertexAttribArray( UniformData.mLocation + i );

						if( Buffer->instanced() && GL33 )
						{
							GL33->glVertexAttribDivisor( UniformData.mLocation + i, 1 );
						}
					}
				}
				break;

			default:
				//qWarning() << "Unknown meta type for VAO:" << QString( QMetaType::typeName( Buffer->type() ) ) << QString::number( Buffer->type() );
				break;
		}

		if( !Buffer->instanced() && GL33 )
		{
			GL33->glVertexAttribDivisor( UniformData.mLocation, 0 );
		}
	}
}


GLuint VertexArrayObjectNode::vaoId() const
{
	return( mVAO.objectId() );
}
