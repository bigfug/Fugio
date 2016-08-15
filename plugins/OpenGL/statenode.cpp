#include "statenode.h"

#include <QPushButton>
#include <QMainWindow>

#include <fugio/global_interface.h>
#include "fugio/context_interface.h"

#include "openglplugin.h"

#include "stateform.h"
#include "statepin.h"

StateNode::StateNode( QSharedPointer<fugio::NodeInterface> pNode ) :
	NodeControlBase( pNode )
{
	mOutputState = pinOutput<OpenGLStateInterface *>( "State", mPinOutputState, PID_OPENGL_STATE );

	mPinOutputState->setDescription( tr( "The output OpenGL state that controls rendering" ) );
}

QWidget *StateNode::gui()
{
	QPushButton		*GUI = new QPushButton( "Edit..." );

	connect( GUI, SIGNAL(clicked()), this, SLOT(onEditClicked()) );

	return( GUI );
}

void StateNode::onEditClicked()
{
	QScopedPointer<StateForm>		Form( new StateForm( mOutputState, mNode->context()->global()->mainWindow() ) );

	if( Form != 0 )
	{
		Form->setModal( true );

		if( Form->exec() == QDialog::Accepted )
		{
			Form->updateState();

			mNode->context()->updateNode( mNode );
		}
	}
}
