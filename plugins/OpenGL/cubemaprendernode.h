#ifndef CUBEMAPRENDERNODE_H
#define CUBEMAPRENDERNODE_H

#include <QObject>
#include <QOpenGLFramebufferObject>

#include <fugio/nodecontrolbase.h>

#include <fugio/core/variant_interface.h>

#include "opengl_includes.h"

#include <fugio/opengl/texture_interface.h>

class CubeMapRenderNode : public fugio::NodeControlBase, public QOpenGLFunctions
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Renders to all six textures in an OpenGL cubemap texture" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Cubemap_Render_(OpenGL)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE CubeMapRenderNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~CubeMapRenderNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputTrigger;
	QSharedPointer<fugio::PinInterface>			 mPinInputRender;
	QSharedPointer<fugio::PinInterface>			 mPinInputMatCam;

	QSharedPointer<fugio::PinInterface>			 mPinOutputMatCam;
	fugio::VariantInterface						*mValOutputMatCam;

	QSharedPointer<fugio::PinInterface>			 mPinOutputTexture;

	static QList<QMatrix4x4>					 mMatCam;
	quint32										 mFBO;
	quint32										 mRBO;
};

#endif // CUBEMAPRENDERNODE_H
