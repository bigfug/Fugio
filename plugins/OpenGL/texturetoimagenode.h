#ifndef TEXTURETOIMAGENODE_H
#define TEXTURETOIMAGENODE_H

#include <QObject>

#if !defined( GL_ES_VERSION_2_0 )
#include <QOpenGLFunctions_2_0>
#include <QOpenGLFunctions_3_2_Core>
#endif

#include "opengl_includes.h"

#include <fugio/nodecontrolbase.h>

#include <fugio/image/image.h>

class TextureToImageNode : public fugio::NodeControlBase, protected QOpenGLFunctions
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Converts an OpenGL texture to an image" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Texture_To_Image_(OpenGL)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE TextureToImageNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~TextureToImageNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputTexture;
	QSharedPointer<fugio::PinInterface>			 mPinOutputImage;
	fugio::VariantInterface								*mValOutputImage;
};


#endif // TEXTURETOIMAGENODE_H
