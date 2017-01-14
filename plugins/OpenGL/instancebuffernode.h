#ifndef INSTANCEBUFFERNODE_H
#define INSTANCEBUFFERNODE_H

#include "opengl_includes.h"

#include <QObject>

#include <fugio/nodecontrolbase.h>

class InstanceBufferNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "OpenGL Instance Buffer" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Instance_Buffer_(OpenGL)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE InstanceBufferNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~InstanceBufferNode( void ) {}

	// NodeControlInterface interface
public:
	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInput;
	QSharedPointer<fugio::PinInterface>			 mPinOutput;
};

#endif // INSTANCEBUFFERNODE_H
