#include "arraytotexturebuffernode.h"

#if !defined( QT_OPENGL_ES )
#include <QOpenGLFunctions_3_1>
#endif

#include <fugio/core/uuid.h>
#include <fugio/opengl/uuid.h>
#include <fugio/performance.h>

#include <fugio/context_interface.h>
#include <fugio/core/variant_interface.h>
//#include <fugio/core/array_interface.h>
#include <fugio/node_signals.h>

#include "openglplugin.h"

ArrayToTextureBufferNode::ArrayToTextureBufferNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_ARRAY,		"da4453c5-c72d-4c40-aff6-453380d0cf13" );
	FUGID( PIN_OUTPUT_BUFFER,	"d68e830c-cc15-430f-a444-02590043006f" );

	mPinInputArray  = pinInput( "Array", PIN_INPUT_ARRAY );

	mPinInputArray->registerPinInputType( PID_ARRAY );

	mValOutputBuffer = pinOutput<fugio::OpenGLTextureInterface *>( "Texture", mPinOutputBuffer, PID_OPENGL_TEXTURE, PIN_OUTPUT_BUFFER );

	mNode->pairPins( mPinInputArray, mPinOutputBuffer );
}

bool ArrayToTextureBufferNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

#if defined( QT_OPENGL_ES )
	mNode->setStatus( fugio::NodeInterface::Error );

	return( false );
#endif

	if( !OpenGLPlugin::hasContextStatic() )
	{
		return( false );
	}

	initializeOpenGLFunctions();

	return( true );
}

void ArrayToTextureBufferNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	if( !pTimeStamp )
	{
		return;
	}

	fugio::Performance	Perf( mNode, "inputsUpdated", pTimeStamp );

	if( !OpenGLPlugin::hasContextStatic() )
	{
		return;
	}

#if !defined( QT_OPENGL_ES )
	QOpenGLFunctions_3_1	*GL31 = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_1>();

	if( !GL31 || !GL31->initializeOpenGLFunctions() )
	{
		return;
	}

	for( fugio::NodeInterface::UuidPair UP : mNode->pairedPins() )
	{
		QSharedPointer<fugio::PinInterface>  PinI = mNode->findPinByLocalId( UP.first );
		QSharedPointer<fugio::PinInterface>  PinO = mNode->findPinByLocalId( UP.second );
		fugio::OpenGLTextureInterface		*BufO = qobject_cast<fugio::OpenGLTextureInterface *>( PinO && PinO->hasControl() ? PinO->control()->qobject() : nullptr );

		if( !PinI || !PinO || !BufO )
		{
			continue;
		}

		if( !PinI->isUpdated( pTimeStamp ) )
		{
			continue;
		}

		fugio::VariantInterface			*A;

		if( ( A = input<fugio::VariantInterface *>( PinI ) ) == nullptr )
		{
			continue;
		}

		GLenum	InternalFormat = 0;

		switch( A->variantType() )
		{
			case QMetaType::UChar:
				InternalFormat = GL_R8UI;
				break;

			case QMetaType::Char:
				InternalFormat = GL_R8I;
				break;

			case QMetaType::UShort:
				InternalFormat = GL_R16UI;
				break;

			case QMetaType::Short:
				InternalFormat = GL_R16I;
				break;

			case QMetaType::UInt:
				InternalFormat = GL_R32UI;
				break;

			case QMetaType::Int:
				InternalFormat = GL_R32I;
				break;

			case QMetaType::Float:
				InternalFormat = GL_R32F;
				break;

			case QMetaType::QPoint:
			case QMetaType::QSize:
				InternalFormat = GL_RG32I;
				break;

			case QMetaType::QVector2D:
			case QMetaType::QSizeF:
			case QMetaType::QPointF:
				InternalFormat = GL_RG32F;
				break;

			case QMetaType::QVector3D:
				InternalFormat = GL_RGB32F;
				break;

			case QMetaType::QVector4D:
				InternalFormat = GL_RGBA32F;
				break;

			default:
				break;
		}

		if( !InternalFormat )
		{
			continue;
		}

		if( BufO->size().x() != A->variantCount() )
		{
			BufO->free();
		}

		if( A->variantCount() <= 0 )
		{
			continue;
		}

		BufO->setTarget( QOpenGLTexture::TargetBuffer );
		BufO->update();

		GLuint		TBO = mBuffers.value( UP.second, 0 );

		if( !TBO )
		{
			glGenBuffers( 1, &TBO );

			mBuffers.insert( UP.second, TBO );
		}

		glBindBuffer( GL_TEXTURE_BUFFER, TBO );
		glBufferData( GL_TEXTURE_BUFFER, A->variantCount() * A->variantStride(), A->variantArray(), GL_STATIC_DRAW );
		glBindBuffer( GL_TEXTURE_BUFFER, 0 );

		BufO->dstTex()->create();

		glBindTexture( GL_TEXTURE_BUFFER, BufO->dstTexId() );

		GL31->glTexBuffer( GL_TEXTURE_BUFFER, InternalFormat, TBO );

		BufO->release();

		OPENGL_DEBUG( mNode->name() );

		pinUpdated( PinO );
	}
#endif
}

QUuid ArrayToTextureBufferNode::pairedPinControlUuid( QSharedPointer<fugio::PinInterface> pPin ) const
{
	return( pPin->direction() == PIN_INPUT ? PID_OPENGL_TEXTURE : QUuid() );
}

QList<QUuid> ArrayToTextureBufferNode::pinAddTypesInput() const
{
	static QList<QUuid> PinLst =
	{
		PID_ARRAY
	};

	return( PinLst );
}

bool ArrayToTextureBufferNode::canAcceptPin( PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_OUTPUT );
}
