#ifndef RENDERNODE_H
#define RENDERNODE_H

#include "opengl_includes.h"

#include <QObject>

#include <fugio/nodecontrolbase.h>

#include <fugio/render_interface.h>
#include <fugio/opengl/node_render_interface.h>

class RenderNode : public fugio::NodeControlBase, public fugio::NodeRenderInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::NodeRenderInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "OpenGL Render" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Render_OpenGL" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE RenderNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~RenderNode( void ) {}

	// NodeControlInterface interface
public:
	virtual bool initialise( void ) Q_DECL_OVERRIDE;

	virtual QList<QUuid> pinAddTypesInput( void ) const Q_DECL_OVERRIDE;

	virtual bool canAcceptPin( fugio::PinInterface *pPin ) const Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	// fugio::NodeRenderInterface interface
public:
	virtual void render( qint64 pTimeStamp, QUuid pSourcePinId ) Q_DECL_OVERRIDE;

protected:
	QSharedPointer<fugio::PinInterface>			 mPinOutput;
	fugio::RenderInterface						*mValOutput;
};

#endif // RENDERNODE_H
