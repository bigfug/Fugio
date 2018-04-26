#include "settingswidget.h"
#include "ui_settingswidget.h"

#include <QFileDialog>

SettingsWidget::SettingsWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::SettingsWidget)
{
	ui->setupUi(this);

	setObjectName( "Freeframe" );
}

SettingsWidget::~SettingsWidget()
{
	delete ui;
}

void SettingsWidget::setPluginPaths( QStringList pPaths )
{
	ui->mPathList->addItems( pPaths );
}

QStringList SettingsWidget::pluginPaths() const
{
	QStringList		PluginPaths;

	for( int i = 0 ; i < ui->mPathList->count() ; i++ )
	{
		QListWidgetItem		*LI = ui->mPathList->item( i );

		PluginPaths << LI->text();
	}

	return( PluginPaths );
}

void SettingsWidget::on_mButtonAdd_clicked()
{
	QString		Path = QFileDialog::getExistingDirectory( nullptr, tr( "Add Freeframe path" ) );

	if( Path.isEmpty() )
	{
		return;
	}

	for( int i = 0 ; i < ui->mPathList->count() ; i++ )
	{
		QListWidgetItem		*LI = ui->mPathList->item( i );

		if( Path == LI->text() )
		{
			return;
		}
	}

	ui->mPathList->addItem( Path );
}

void SettingsWidget::on_mButtonRemove_clicked()
{
	QListWidgetItem		*LI = ui->mPathList->currentItem();

	if( LI )
	{
		delete LI;
	}

	ui->mButtonRemove->setEnabled( false );
}

void SettingsWidget::on_mPathList_itemClicked(QListWidgetItem *item)
{
	ui->mButtonRemove->setEnabled( item );
}
