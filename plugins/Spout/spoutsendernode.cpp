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
	memset( mName, 0, sizeof( mName ) );

	mPinTexture = pinInput( "Texture" );

	mPinName    = pinInput( "Name" );

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

	mSender.SendTexture( TexSrc->srcTexId(), TexSrc->target(), mWidth, mHeight );
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
