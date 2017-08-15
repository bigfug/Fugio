#ifndef VIEWPORTMATRIXNODE_H
#define VIEWPORTMATRIXNODE_H

#include "opengl_includes.h"

#include <QObject>
#include <QRectF>

#include <fugio/nodecontrolbase.h>
#include <fugio/render_interface.h>
#include <fugio/opengl/node_render_interface.h>

class ViewportMatrixNode : public fugio::NodeControlBase, public fugio::NodeRenderInterface, private QOpenGLFunctions
{
	Q_OBJECT
	Q_INTERFACES( fugio::NodeRenderInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Renders multiple viewports" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Viewport_Matrix_(OpenGL)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE ViewportMatrixNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~ViewportMatrixNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

	virtual QList<QUuid> pinAddTypesInput() const;

	virtual bool canAcceptPin( fugio::PinInterface *pPin ) const;

	// fugio::NodeRenderInterface

	virtual void render( qint64 pTimeStamp, QUuid pSourcePinId );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinX;
	QSharedPointer<fugio::PinInterface>			 mPinY;

	QSharedPointer<fugio::PinInterface>			 mPinInputRender;
	QSharedPointer<fugio::PinInterface>			 mPinInputProjections;
	QSharedPointer<fugio::PinInterface>			 mPinInputViews;

	QSharedPointer<fugio::PinInterface>			 mPinOutput;
	fugio::RenderInterface						*mValOutput;

	QSharedPointer<fugio::PinInterface>			 mPinOutputProjection;
	fugio::VariantInterface						*mValOutputProjection;

	QSharedPointer<fugio::PinInterface>			 mPinOutputView;
	fugio::VariantInterface						*mValOutputView;

	QSharedPointer<fugio::PinInterface>			 mPinOutputIndex;
	fugio::VariantInterface						*mValOutputIndex;

	bool										 mUpdateViewport;
};

#endif // VIEWPORTMATRIXNODE_H
