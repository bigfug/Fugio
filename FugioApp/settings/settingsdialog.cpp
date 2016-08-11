#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QFileDialog>

#include "app.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::SettingsDialog)
{
	ui->setupUi( this );

	connect( this, SIGNAL(accepted()), this, SLOT(dialogAccepted()) );

	ui->mSnippetsDirectory->setText( gApp->userSnippetsDirectory() );
	ui->mSnippetsDirectory->setReadOnly( true );
}

SettingsDialog::~SettingsDialog()
{
	delete ui;
}

void SettingsDialog::on_mSnippetsSelect_clicked()
{
	QString	NewDir = QFileDialog::getExistingDirectory( this, tr( "Choose directory for Snippets" ), ui->mSnippetsDirectory->text() );

	if( !NewDir.isEmpty() )
	{
		ui->mSnippetsDirectory->setText( NewDir );
	}
}

void SettingsDialog::dialogAccepted()
{
	gApp->setUserSnippetsDirectory( ui->mSnippetsDirectory->text() );
}
