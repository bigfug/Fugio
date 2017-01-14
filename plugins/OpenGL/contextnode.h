#ifndef CONTEXTNODE_H
#define CONTEXTNODE_H

#include "opengl_includes.h"

#include <fugio/nodecontrolbase.h>

#if defined( Q_OS_RASPBERRY_PI )
#include "deviceopengloutputrpi.h"
#else
#include "deviceopengloutput.h"
#endif

class ContextNode : public fugio::NodeControlBase
{
	Q_OBJECT

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Creates an OpenGL context" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Context_(OpenGL)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit ContextNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~ContextNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual bool initialise( void ) Q_DECL_OVERRIDE;

	virtual bool deinitialise( void ) Q_DECL_OVERRIDE;

private:
	QSharedPointer<DeviceOpenGLOutput>	 mOutput;
};


#endif // CONTEXTNODE_H
