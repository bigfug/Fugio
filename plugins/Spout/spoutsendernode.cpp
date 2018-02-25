#include "spoutsendernode.h"

#include <fugio/core/uuid.h>
#include <fugio/opengl/uuid.h>
#include <fugio/spout/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/context_signals.h>

#include "spoutplugin.h"
#include "spoutpin.h"
#include "spoutreceiverform.h"

SpoutSenderNode::SpoutSenderNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mWidth( 0 ), mHeight( 0 )
{
	FUGID( PIN_INPUT_TEXTURE, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_NAME, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_INPUT_FLIP, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );

	memset( mName, 0, sizeof( mName ) );

	mPinTexture = pinInput( "Texture", PIN_INPUT_TEXTURE );

	mPinName    = pinInput( "Name", PIN_INPUT_NAME );

	mPinInputFlip = pinInput( "Flip", PIN_INPUT_FLIP );

	mPinInputFlip->setValue( true );

	mPinTexture->registerPinInputType( PID_OPENGL_TEXTURE );

	mPinName->registerPinInputType( PID_STRING );

	static int		SenderId = 0;

	mPinName->setValue( QString( "Fugio-Sender-%1" ).arg( SenderId++ ) );
}

void SpoutSenderNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	char					 NewNam[ 256 ];

#if defined( Q_OS_WIN )
	strcpy_s( NewNam, variant( mPinName ).toString().toLatin1().constData() );
#else
	strcpy( NewNam, variant( mPinName ).toString().toLatin1().constData() );
#endif

	if( !NewNam[ 0 ] )
	{
		return;
	}

	fugio::OpenGLTextureInterface		*TexSrc = input<fugio::OpenGLTextureInterface *>( mPinTexture );

	if( !TexSrc )
	{
		return;
	}

	if( TexSrc->target() != GL_TEXTURE_2D )
	{
		return;
	}

	QVector3D			 TexSiz = TexSrc->size();

	if( !TexSrc->srcTexId() || !TexSiz.x() || !TexSiz.y() )
	{
		return;
	}

#if defined( SPOUT_SUPPORTED )
	if( strcmp( mName, NewNam ) || TexSiz[ 0 ] != mWidth || TexSiz[ 1 ] != mHeight )
	{
		if( mWidth > 0 && mHeight > 0 )
		{
			mSender.ReleaseSender();

			memset( mName, 0, sizeof( mName ) );

			mWidth = mHeight = 0;
		}

		if( !mSender.CreateSender( NewNam, TexSiz[ 0 ], TexSiz[ 1 ] ) )
		{
			return;
		}

#if defined( Q_OS_WIN )
		strcpy_s( mName, NewNam );
#else
		strcpy( mName, NewNam );
#endif

		mWidth  = TexSiz[ 0 ];
		mHeight = TexSiz[ 1 ];
	}

	mSender.SendTexture( TexSrc->srcTexId(), TexSrc->target(), mWidth, mHeight, variant<bool>( mPinInputFlip ) );
#endif
}

bool SpoutSenderNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

#if !defined( SPOUT_SUPPORTED )
	if( true )
	{
		mNode->setStatus( fugio::NodeInterface::Error );
		mNode->setStatusMessage( tr( "Spout not supported on this platform" ) );

		return( false );
	}
#endif

	if( !SpoutPlugin::hasOpenGLContextStatic() )
	{
		return( false );
	}

	//connect( mNode->context()->context(), SIGNAL(frameInitialise()), this, SLOT(onContextFrame()) );

	return( true );
}

bool SpoutSenderNode::deinitialise()
{
	//disconnect( mNode->context()->context(), SIGNAL(frameInitialise()), this, SLOT(onContextFrame()) );

	return( NodeControlBase::deinitialise() );
}
