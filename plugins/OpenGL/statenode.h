#ifndef STATENODE_H
#define STATENODE_H

#include "opengl_includes.h"

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>

#include <fugio/opengl/state_interface.h>

#include <fugio/opengl/uuid.h>

#include <fugio/nodecontrolbase.h>

class StateNode : public fugio::NodeControlBase
{
	Q_OBJECT

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Control how OpenGL draws" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "State_(OpenGL)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit StateNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~StateNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface interface

public:
	virtual QWidget *gui( void );

private slots:
	void onEditClicked( void );

private:
	QSharedPointer<fugio::PinInterface>		 mPinProjection;
	QSharedPointer<fugio::PinInterface>		 mPinModelView;

	QSharedPointer<fugio::PinInterface>		 mPinOutputState;
	fugio::OpenGLStateInterface				*mOutputState;

};

#endif // STATENODE_H
