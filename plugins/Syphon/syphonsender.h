#ifndef SYPHONSENDER_H
#define SYPHONSENDER_H

#include <string>
#include <list>

class SyphonSender
{
public:
	SyphonSender();

	~SyphonSender( void );

	void setServerName( const std::string &pServerName );

	std::string serverName( void ) const
	{
		return( mServerName );
	}

	void publishTexture( unsigned int pTexId, unsigned int pTarget, int pWidth, int pHeight, bool pFlipped );

	bool bindToDrawFrameOfSize( int pWidth, int pHeight );

	void unbindAndPublish( void );

private:
	void					*mServer;
	std::string				 mServerName;
};

#endif // SYPHONSENDER_H
