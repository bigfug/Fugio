#include "syphonsender.h"

#if defined( SYPHON_SUPPORTED )
#include <Syphon/Syphon.h>
#include <Syphon/SyphonClient.h>
#include <Syphon/SyphonImage.h>
#include <Syphon/SyphonServer.h>
#include <Syphon/SyphonServerDirectory.h>
#endif

SyphonSender::SyphonSender()
	: mServer( 0 )
{

}

SyphonSender::~SyphonSender()
{
#if defined( SYPHON_SUPPORTED )
	if( mServer )
	{
		[(SyphonServer *)mServer stop];
		[(SyphonServer *)mServer release];

		mServer = 0;
	}
#endif
}

void SyphonSender::setServerName( const std::string &pServerName )
{
	if( pServerName == mServerName )
	{
		return;
	}

#if defined( SYPHON_SUPPORTED )
	if( mServer )
	{
		[(SyphonServer *)mServer stop];
		[(SyphonServer *)mServer release];

		mServer = 0;
	}

	CGLContextObj		ContextObj = CGLGetCurrentContext();

	if( ContextObj )
	{
		mServer = [[SyphonServer alloc] initWithName:[NSString stringWithUTF8String:pServerName.c_str()] context:ContextObj options:nil];
	}
#endif

	mServerName = pServerName;
}

void SyphonSender::publishTexture( unsigned int pTexId, unsigned int pTarget, int pWidth, int pHeight, bool pFlipped )
{
#if defined( SYPHON_SUPPORTED )
	if( !mServer )
	{
		return;
	}

	NSRect			R = NSMakeRect( 0, 0, pWidth, pHeight );
	NSSize			S = NSMakeSize( pWidth, pHeight );

	[(SyphonServer *)mServer publishFrameTexture:pTexId textureTarget:pTarget imageRegion:R textureDimensions:S flipped:pFlipped];

#endif
}

bool SyphonSender::bindToDrawFrameOfSize(int pWidth, int pHeight)
{
#if defined( SYPHON_SUPPORTED )
	if( !mServer )
	{
		return( false );
	}

	return( [(SyphonServer *)mServer bindToDrawFrameOfSize:NSMakeSize( pWidth, pHeight ) ] );
#else
	return( false );
#endif
}

void SyphonSender::unbindAndPublish()
{
#if defined( SYPHON_SUPPORTED )
	if( mServer )
	{
		[(SyphonServer *)mServer unbindAndPublish];
	}
#endif
}
