#include "settingsform.h"
#include "ui_settingsform.h"

#include <QFileDialog>

SettingsForm::SettingsForm(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::SettingsForm)
{
	ui->setupUi(this);

	connect( this, SIGNAL(pathChanged(QString)), ui->mISFPath, SLOT(setText(QString)) );
}

SettingsForm::~SettingsForm()
{
	delete ui;
}

void SettingsForm::on_mISFButton_clicked()
{
	QString	NewDir = QFileDialog::getExistingDirectory( nullptr, tr( "Choose ISF directory" ), ui->mISFPath->text() );

	if( !NewDir.isNull() )
	{
		setPath( NewDir );
	}
}
