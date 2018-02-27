#ifndef FFGLNODE_H
#define FFGLNODE_H

#include <QOpenGLFunctions>

#include <fugio/nodecontrolbase.h>

#include <fugio/core/variant_interface.h>

#include <fugio/opengl/texture_interface.h>

#include <fugio/opengl/node_render_interface.h>

#include "freeframelibrary.h"

class FFGLNode : public fugio::NodeControlBase, public fugio::NodeRenderInterface, protected QOpenGLFunctions
{
	Q_OBJECT
	Q_INTERFACES( fugio::NodeRenderInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "FFGL" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "FFGL_(FreeFrame)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE FFGLNode( QSharedPointer<fugio::NodeInterface> pNode);

	virtual ~FFGLNode( void ) {}

	// InterfaceNodeControl interface

	virtual bool initialise( void ) Q_DECL_OVERRIDE;

	virtual bool deinitialise( void ) Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	// NodeRenderInterface interface
public:
	virtual void render( qint64 pTimeStamp, QUuid pSourcePinId ) Q_DECL_OVERRIDE;

protected:
	void initialiseInstance( QPoint pPoint, QSize pSize );

	void render( void );

protected:
	FreeframeLibrary								*mLibrary;
	FFInstanceID									 mInstanceId;

	QSharedPointer<fugio::PinInterface>				 mPinInputRender;

	QSharedPointer<fugio::PinInterface>				 mPinOutputRender;
	fugio::NodeRenderInterface						*mValOutputRender;

	QVector<QSharedPointer<fugio::PinInterface>>	 mInputs;
	QVector<QSharedPointer<fugio::PinInterface>>	 mParams;

	QPoint											 mPoint;
	QSize											 mSize;
};

#endif // FFGLNODE_H
