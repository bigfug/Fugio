#ifndef SYPHONRECEIVER_H
#define SYPHONRECEIVER_H

#include <string>
#include <list>
#include <map>

class SyphonReceiver
{
public:
	SyphonReceiver( void );

	virtual ~SyphonReceiver( void );

	void setServerName( const std::string &pServerName );
	void setServerUuid( const std::string &pServerUuid );

	std::string serverUuid( void ) const
	{
		return( mServerUuid );
	}

	std::string serverName( void ) const
	{
		return( mServerName );
	}

	bool hasFrameAvailable( void ) const
	{
		return( mFrameAvailable );
	}

	int textureId( void ) const
	{
		return( mTextureId );
	}

	int textureWidth( void ) const
	{
		return( mTextureWidth );
	}

	int textureHeight( void ) const
	{
		return( mTextureHeight );
	}

	void bind( void );
	void release( void );

	static std::map<std::string,std::string> serverList( void );

protected:
	std::string				 mServerUuid;
	std::string				 mServerName;
	bool					 mFrameAvailable;
	int						 mTextureId;
	int						 mTextureWidth;
	int						 mTextureHeight;
	void					*mClient;
	void					*mImage;
};

#endif // SYPHONRECEIVER_H
