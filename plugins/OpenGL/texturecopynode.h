#ifndef TEXTURECOPYNODE_H
#define TEXTURECOPYNODE_H

#include "opengl_includes.h"

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>

#include <fugio/opengl/texture_interface.h>

#include <fugio/opengl/uuid.h>

#include <fugio/nodecontrolbase.h>

class TextureCopyNode : public fugio::NodeControlBase
{
	Q_OBJECT

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Copies one OpenGL texture to another" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Texture_Copy_(OpenGL)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit TextureCopyNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~TextureCopyNode( void );

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual QUuid uuid( void )
	{
		const static QUuid uuid = NID_OPENGL_TEXTURE_COPY;

		return( uuid );
	}

	virtual void inputsUpdated( qint64 pTimeStamp );

private:
	QSharedPointer<fugio::PinInterface>			 mPinTexSrc;

	QSharedPointer<fugio::PinInterface>			 mPinTexDst;
	fugio::OpenGLTextureInterface				*mTexDst;

	GLuint										 mFBO;
};

#endif // TEXTURECOPYNODE_H
