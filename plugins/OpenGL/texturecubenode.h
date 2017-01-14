#ifndef TEXTURECUBENODE_H
#define TEXTURECUBENODE_H

#include "opengl_includes.h"

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>

#include <fugio/opengl/texture_interface.h>

#include <fugio/opengl/uuid.h>

#include <fugio/nodecontrolbase.h>

class TextureCubeNode : public fugio::NodeControlBase
{
	Q_OBJECT

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Creates a texture cube from a source image" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Texture_Cube_(OpenGL)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit TextureCubeNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~TextureCubeNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface interface
public:
	virtual bool initialise();

	virtual void inputsUpdated( qint64 pTimeStamp );

private:
	void processTexture( QSharedPointer<fugio::PinInterface> pPin, int pCubeMapIndex );

private:
	QSharedPointer<fugio::PinInterface>			 mPinImgTop;
	QSharedPointer<fugio::PinInterface>			 mPinImgBottom;
	QSharedPointer<fugio::PinInterface>			 mPinImgLeft;
	QSharedPointer<fugio::PinInterface>			 mPinImgRight;
	QSharedPointer<fugio::PinInterface>			 mPinImgFront;
	QSharedPointer<fugio::PinInterface>			 mPinImgBack;

	QSharedPointer<fugio::PinInterface>			 mPinTexDst;
	fugio::OpenGLTextureInterface						*mTexDst;
};

#endif // TEXTURECUBENODE_H
