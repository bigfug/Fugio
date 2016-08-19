#ifndef VIEWPORTNODE_H
#define VIEWPORTNODE_H

#include "opengl_includes.h"

#include <QObject>
#include <QRectF>

#include <fugio/nodecontrolbase.h>

#include <fugio/render_interface.h>
#include <fugio/opengl/node_render_interface.h>

class ViewportNode : public fugio::NodeControlBase, public fugio::NodeRenderInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::NodeRenderInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Limit OpenGL to drawing in part of the output window" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Viewport_OpenGL" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE ViewportNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~ViewportNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

	virtual QList<QUuid> pinAddTypesInput() const;

	virtual bool canAcceptPin( fugio::PinInterface *pPin ) const;

	// fugio::NodeRenderInterface

	virtual void render( qint64 pTimeStamp, QUuid pSourcePinId );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinX;
	QSharedPointer<fugio::PinInterface>			 mPinY;
	QSharedPointer<fugio::PinInterface>			 mPinW;
	QSharedPointer<fugio::PinInterface>			 mPinH;

	QSharedPointer<fugio::PinInterface>			 mPinOutput;
	fugio::RenderInterface						*mValOutput;

	bool										 mUpdateViewport;
	QRectF										 mVP;
};


#endif // VIEWPORTNODE_H
