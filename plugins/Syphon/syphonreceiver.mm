#include "syphonreceiver.h"

#if defined( SYPHON_SUPPORTED )
#include <Syphon/Syphon.h>
#include <Syphon/SyphonClient.h>
#include <Syphon/SyphonImage.h>
#include <Syphon/SyphonServer.h>
#include <Syphon/SyphonServerDirectory.h>
#endif

SyphonReceiver::SyphonReceiver()
{
#if defined( SYPHON_SUPPORTED )
	mClient = 0;
	mImage  = 0;
#endif
}

SyphonReceiver::~SyphonReceiver()
{
#if defined( SYPHON_SUPPORTED )
	if( mClient )
	{
		[(SyphonClient *)mClient stop];
		[(SyphonClient *)mClient release];

		mClient = 0;
	}
#endif
}

void SyphonReceiver::setServerName( const std::string &pServerName )
{
#if defined( SYPHON_SUPPORTED )
	if( mClient )
	{
		[(SyphonClient *)mClient stop];
		[(SyphonClient *)mClient release];

		mClient = 0;
	}

	if( pServerName.empty() )
	{
		return;
	}

	for( NSDictionary *Server in [[SyphonServerDirectory sharedDirectory] servers] )
	{
		NSString		*UUID = [Server objectForKey:SyphonServerDescriptionUUIDKey];
		NSString		*Name = [Server objectForKey:SyphonServerDescriptionAppNameKey];

		std::string	ServerUuid = std::string( [UUID UTF8String] );
		std::string	ServerName = std::string( [Name UTF8String] );

		if( ServerName != pServerName )
		{
			continue;
		}

		mServerName = ServerName;
		mServerUuid = ServerUuid;

		mClient = [[SyphonClient alloc] initWithServerDescription:Server options:nil newFrameHandler:^(SyphonClient *)
		{
			mFrameAvailable = true;
		}];
	}
#endif
}

void SyphonReceiver::setServerUuid( const std::string &pServerUuid )
{
#if defined( SYPHON_SUPPORTED )
	if( mClient )
	{
		[(SyphonClient *)mClient stop];
		[(SyphonClient *)mClient release];

		mClient = 0;
	}

	if( pServerUuid.empty() )
	{
		return;
	}

	for( NSDictionary *Server in [[SyphonServerDirectory sharedDirectory] servers] )
	{
		NSString		*UUID = [Server objectForKey:SyphonServerDescriptionUUIDKey];
		NSString		*Name = [Server objectForKey:SyphonServerDescriptionAppNameKey];

		std::string	ServerUuid = std::string( [UUID UTF8String] );
		std::string	ServerName = std::string( [Name UTF8String] );

		if( ServerUuid != pServerUuid )
		{
			continue;
		}

		mServerName = ServerName;
		mServerUuid = ServerUuid;

		mClient = [[SyphonClient alloc] initWithServerDescription:Server options:nil newFrameHandler:^(SyphonClient *)
		{
			mFrameAvailable = true;
		}];
	}
#endif
}

void SyphonReceiver::bind()
{
	if( !mClient )
	{
		return;
	}

	mImage = [(SyphonClient *)mClient newFrameImageForContext:CGLGetCurrentContext()];

	if( mImage )
	{
		mTextureId     = ((SyphonImage *)mImage).textureName;
		mTextureWidth  = ((SyphonImage *)mImage).textureSize.width;
		mTextureHeight = ((SyphonImage *)mImage).textureSize.height;
	}
}

void SyphonReceiver::release()
{
	if( mImage )
	{
		[(SyphonImage *)mImage release];

		mImage = 0;
	}

	mFrameAvailable = false;
}

std::map<std::string,std::string> SyphonReceiver::serverList()
{
	std::map<std::string,std::string>		ServerMap;

#if defined( SYPHON_SUPPORTED )
	for( NSDictionary *Server in [[SyphonServerDirectory sharedDirectory] servers] )
	{
		NSString		*UUID = [Server objectForKey:SyphonServerDescriptionUUIDKey];
		NSString		*Name = [Server objectForKey:SyphonServerDescriptionAppNameKey];

		std::string	ServerUuid = std::string( [UUID UTF8String] );
		std::string	ServerName = std::string( [Name UTF8String] );

		ServerMap.insert( std::pair<std::string,std::string>( ServerUuid, ServerName ) );
	}
#endif

	return( ServerMap );
}
