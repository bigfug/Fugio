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

	mValOutputImage = pinOutput<fugio::ImageInterface *>( "Image", mPinOutputImage, PID_IMAGE, ID_IMAGE );
}

void TextureToImageNode::inputsUpdated( qint64 pTimeStamp )
{
	OpenGLTextureInterface		*TexInf = input<OpenGLTextureInterface *>( mPinInputTexture );

	if( !TexInf || !TexInf->dstTexId() )
	{
		return;
	}

	fugio::Performance		Perf( mNode, "inputsUpdated", pTimeStamp );

	const QVector3D	TexSze = TexInf->size();

	if( mValOutputImage->size() != QSize( TexSze.x(), TexSze.y() ) )
	{
#if defined( GL_ES_VERSION_2_0 )
		mValOutputImage->setFormat( fugio::ImageInterface::FORMAT_RGBA8 );
#else
		mValOutputImage->setFormat( fugio::ImageInterface::FORMAT_BGRA8 );
#endif
		mValOutputImage->setSize( TexSze.x(), TexSze.y() );
		mValOutputImage->setLineSize( 0, TexSze.x() * 4 );
	}

	glBindFramebuffer( GL_FRAMEBUFFER, TexInf->fbo() );

#if defined( GL_ES_VERSION_2_0 )
	glReadPixels( 0, 0, TexSze.x(), TexSze.y(), GL_RGBA, GL_UNSIGNED_BYTE, mValOutputImage->internalBuffer( 0 ) );
#else
	glReadPixels( 0, 0, TexSze.x(), TexSze.y(), GL_BGRA, GL_UNSIGNED_BYTE, mValOutputImage->internalBuffer( 0 ) );
#endif

	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	pinUpdated( mPinOutputImage );
}
