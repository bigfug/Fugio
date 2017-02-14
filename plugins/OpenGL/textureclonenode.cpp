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

	mValTexDst->setTarget( GL_TEXTURE_2D );

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
	fugio::Performance	Perf( mNode, "inputsUpdated", pTimeStamp );

	NodeControlBase::inputsUpdated( pTimeStamp );

	if( !OpenGLPlugin::hasContextStatic() )
	{
		return;
	}

	if( mPinTexClone->isUpdated( pTimeStamp ) )
	{
		fugio::OpenGLTextureInterface	*TexSrc = input<fugio::OpenGLTextureInterface *>( mPinTexClone );

		if( TexSrc )
		{
			fugio::OpenGLTextureInterface	*TexDst = mValTexDst;
			QVector3D						 SrcSze = TexSrc->size();

			quint32				 SrcCnt = 0;

			for( int i = 0 ; i < 3 ; i++ )
			{
				SrcCnt += SrcSze[ i ];
			}

			if( SrcCnt == 0 )
			{
				return;
			}

			QVector3D			 DstSze = TexDst->size();

			quint32				 DstCnt = 0;

			for( int i = 0 ; i < 3 ; i++ )
			{
				DstCnt += ( SrcSze[ i ] != DstSze[ i ] ? 1 : 0 );
			}

			if( DstCnt )
			{
				TexDst->setCompare( TexSrc->compare() );
				TexDst->setDoubleBuffered( TexSrc->isDoubleBuffered() );
				TexDst->setFilter( TexSrc->filterMin(), TexSrc->filterMag() );
				TexDst->setGenMipMaps( TexSrc->genMipMaps() );
				TexDst->setSize( SrcSze[ 0 ], SrcSze[ 1 ], SrcSze[ 2 ] );
				TexDst->setFormat( TexSrc->format() );
				TexDst->setInternalFormat( TexSrc->internalFormat() );
				TexDst->setTarget( TexSrc->target() );
				TexDst->setType( TexSrc->type() );
				TexDst->setWrap( TexSrc->wrapS(), TexSrc->wrapT(), TexSrc->wrapR() );

				TexDst->update();

				pinUpdated( mPinTexDst );
			}
		}
	}

	if( mPinTexSrc->isUpdated( pTimeStamp ) )
	{
		pinUpdated( mPinTexDst );
	}
}

