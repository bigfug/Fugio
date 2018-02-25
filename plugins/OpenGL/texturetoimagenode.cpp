#include "texturetoimagenode.h"

#include <fugio/performance.h>

#include <fugio/opengl/uuid.h>
#include <fugio/image/uuid.h>

#include "opengl_includes.h"

#include <fugio/opengl/texture_interface.h>

using namespace fugio;

TextureToImageNode::TextureToImageNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( ID_TEXTURE,	"7A53930F-A666-45CC-8828-BF9AC488AC8D" );
	FUGID( ID_IMAGE,	"F6B44E31-15B0-4052-8D66-9EF3BD3FBE47" );

	mPinInputTexture = pinInput( "Texture", ID_TEXTURE );

	mPinInputTexture->registerPinInputType( PID_OPENGL_TEXTURE );

	mValOutputImage = pinOutput<fugio::VariantInterface *>( "Image", mPinOutputImage, PID_IMAGE, ID_IMAGE );
}

void TextureToImageNode::inputsUpdated( qint64 pTimeStamp )
{
	OpenGLTextureInterface		*TexInf = input<OpenGLTextureInterface *>( mPinInputTexture );

	if( !TexInf || !TexInf->srcTexId() )
	{
		return;
	}

	fugio::Performance		Perf( mNode, "inputsUpdated", pTimeStamp );

	initializeOpenGLFunctions();

	const QVector3D	TexSze = TexInf->size();

	fugio::Image			Output = mValOutputImage->variant().value<fugio::Image>();

	if( Output.size() != QSize( TexSze.x(), TexSze.y() ) )
	{
#if defined( GL_ES_VERSION_2_0 )
		Output.setFormat( fugio::ImageFormat::RGBA8 );
#else
		Output.setFormat( fugio::ImageFormat::BGRA8 );
#endif
		Output.setSize( TexSze.x(), TexSze.y() );
		Output.setLineSize( 0, TexSze.x() * 4 );
	}

	if( Output.isValid() )
	{
#if !defined( GL_ES_VERSION_2_0 )
//		QOpenGLFunctions_2_0	*GL20 = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_2_0>();

//		if( GL20 && GL20->initializeOpenGLFunctions() )
//		{
//			qDebug() << "GL20";

//			GL20->glGetTexImage( TexInf->target(), 0, GL_BGR, GL_UNSIGNED_BYTE, Output.internalBuffer( 0 ) );
//		}

		QOpenGLFunctions_3_2_Core	*GL32 = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_2_Core>();

		if( GL32 && GL32->initializeOpenGLFunctions() )
		{
			GL32->glActiveTexture( GL_TEXTURE0 );

			TexInf->srcBind();

			GL32->glGetTexImage( TexInf->target(), 0, GL_BGRA, GL_UNSIGNED_BYTE, Output.internalBuffer( 0 ) );

			TexInf->release();
		}
#endif

		pinUpdated( mPinOutputImage );
	}
}
