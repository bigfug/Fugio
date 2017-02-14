#ifndef TEXTURECLONENODE_H
#define TEXTURECLONENODE_H

#include "opengl_includes.h"

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>

#include <fugio/opengl/texture_interface.h>

#include <fugio/opengl/uuid.h>

#include <fugio/nodecontrolbase.h>

class TextureCloneNode : public fugio::NodeControlBase
{
	Q_OBJECT

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Creates a new OpenGL texture based on an input texture" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Texture_Clone_(OpenGL)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit TextureCloneNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~TextureCloneNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual bool initialise() Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

private:
	QSharedPointer<fugio::PinInterface>			 mPinTexClone;

	QSharedPointer<fugio::PinInterface>			 mPinTexSrc;

	QSharedPointer<fugio::PinInterface>			 mPinTexDst;
	fugio::OpenGLTextureInterface				*mValTexDst;
};

#endif // TEXTURECLONENODE_H
