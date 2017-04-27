#include "statenode.h"

#include <QPushButton>
#include <QMainWindow>

#include <fugio/global_interface.h>
#include "fugio/context_interface.h"
#include <fugio/editor_interface.h>

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
	fugio::EditorInterface	*EI = qobject_cast<fugio::EditorInterface *>( mNode->context()->global()->findInterface( IID_EDITOR ) );

	QScopedPointer<StateForm>		Form( new StateForm( mOutputState, EI ? EI->mainWindow() : nullptr ) );

	if( Form )
	{
		Form->setModal( true );

		if( Form->exec() == QDialog::Accepted )
		{
			Form->updateState();

			mNode->context()->updateNode( mNode );
		}
	}
}
