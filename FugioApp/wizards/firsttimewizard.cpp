#include "firsttimewizard.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QFile>

FirstTimeWizard::FirstTimeWizard()
{
	QWizardPage		*Page = new QWizardPage();

	Page->setTitle( "Fugio" );

	QVBoxLayout	*Layout = new QVBoxLayout();

	QFile	HTML( ":/html/firsttimewizard.html" );

	QString	 TxtLst;

	if( HTML.open( QFile::ReadOnly | QFile::Text ) )
	{
		TxtLst = HTML.readAll();

		HTML.close();
	}

	QLabel		*Label = new QLabel( TxtLst );

	Label->setOpenExternalLinks( true );

	Layout->addWidget( Label );

	Page->setLayout( Layout );

	addPage( Page );
}

FirstTimeWizard::~FirstTimeWizard()
{

}

