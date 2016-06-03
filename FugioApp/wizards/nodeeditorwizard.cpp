#include "nodeeditorwizard.h"

#include <QLabel>
#include <QVBoxLayout>

NodeEditorWizard::NodeEditorWizard( QWidget *pParent)
	: QWizard( pParent )
{
	QWizardPage		*Page = new QWizardPage();

	Page->setTitle( tr( "Node Editor" ) );

	QVBoxLayout	*Layout = new QVBoxLayout();

	QStringList	 TxtLst;

	TxtLst << tr( "<h1>This is the Fugio Node Editor</h1>" );
	TxtLst << "";
	TxtLst << tr( "<h2>To add a new node</h2>" );
	TxtLst << "";
	TxtLst << tr( "<ol><li>Double click on the name in the Node Window</li>" );
	TxtLst << tr( "<li>Drag and drop a name from the Node Window to the Node Editor</li>" );
	TxtLst << tr( "<li>Double-click in the Node Editor</li></ol>" );
	TxtLst << "";
	TxtLst << tr( "<p>See <a href=\"http://wiki.bigfug.com/Node_Editor?utm_source=fugio&utm_medium=node-editor-wizard\">http://wiki.bigfug.com/Node_Editor</a> for more information.</p>" );

	QLabel		*Label = new QLabel( TxtLst.join( '\n' ) );

	Label->setOpenExternalLinks( true );

	Layout->addWidget( Label );

	Page->setLayout( Layout );

	addPage( Page );
}

NodeEditorWizard::~NodeEditorWizard()
{

}

