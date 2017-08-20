#include "textureclonenode.h"

#include <fugio/core/uuid.h>
#include <fugio/context_interface.h>
#include <fugio/performance.h>

#include <QSurfaceFormat>

#include "openglplugin.h"

TextureCloneNode::TextureCloneNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_CLONE, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_TEXTURE, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_OUTPUT_TEXTURE, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );

	mPinTexClone = pinInput( "Clone", PIN_INPUT_CLONE );

	mPinTexSrc = pinInput( "Texture", PIN_INPUT_TEXTURE );

	mValTexDst = pinOutput<fugio::OpenGLTextureInterface *>( "Texture", mPinTexDst, PID_OPENGL_TEXTURE, PIN_OUTPUT_TEXTURE );

	mPinTexClone->setDescription( tr( "The OpenGL Texture to clone" ) );

	mPinTexSrc->setDescription( tr( "The source OpenGL Texture" ) );

	mPinTexDst->setDescription( tr( "The destination OpenGL Texture" ) );

	mValTexDst->setTarget( QOpenGLTexture::Target2D );

	mPinTexSrc->setControl( mPinTexDst->control() );
}

bool TextureCloneNode::initialise()
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

void TextureCloneNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	if( !OpenGLPlugin::hasContextStatic() )
	{
		return;
	}

	fugio::OpenGLTextureInterface	*TexSrc = input<fugio::OpenGLTextureInterface *>( mPinTexClone );
	fugio::OpenGLTextureInterface	*TexDst = mValTexDst;

	if( TexSrc )
	{
		fugio::OpenGLTextureDescription		SrcDsc = TexSrc->textureDescription();
		fugio::OpenGLTextureDescription		DstDsc = TexDst->textureDescription();

		if( memcmp( &SrcDsc, &DstDsc, sizeof( fugio::OpenGLTextureDescription ) ) != 0 )
		{
			TexDst->setTextureDescription( SrcDsc );

			TexDst->update();
		}
	}

	if( !mValTexDst->dstTexId() )
	{
		return;
	}

	if( mPinTexSrc->isUpdated( pTimeStamp ) )
	{
		pinUpdated( mPinTexDst );
	}
}

