#include "pluginsform.h"
#include "app.h"
#include "mainwindow.h"
#include "qcombobox.h"
#include "ui_pluginsform.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QCheckBox>

#include <globalprivate.h>

PluginsForm::PluginsForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PluginsForm)
{
    ui->setupUi(this);

	ui->mRepoInfo->hide();

	qDebug() << mPluginCache.pluginConfigFilename();

	rebuildPluginInformation();
}

PluginsForm::~PluginsForm()
{
    delete ui;
}

void PluginsForm::on_mSourceList_itemSelectionChanged()
{
	QListWidgetItem		*item = ui->mSourceList->currentItem();

	if( !item )
	{
		ui->mButtonSourceRemove->setEnabled( false );
		ui->mButtonSourceUpdate->setEnabled( false );

		ui->mRepoInfo->hide();

		return;
	}

	ui->mRepoInfo->show();

	ui->mButtonSourceRemove->setEnabled( true );
	ui->mButtonSourceUpdate->setEnabled( true );

	PluginRepoManifest	RM( mPluginCache.repoCacheDirectory().filePath( mPluginCache.repoManifestFilename( item->text() ) ), "win64" );

	ui->mRepoIdent->setText( RM.identifier() );
	ui->mRepoAuthor->setText( RM.author() );
	ui->mRepoContact->setText( QString( "<a href=\"%1\">%1</a>" ).arg( RM.contact().toString() ) );
	ui->mRepoHomepage->setText( QString( "<a href=\"%1\">%1</a>" ).arg( RM.homepage().toString() ) );
	ui->mRepoDescription->setPlainText( RM.description() );
}


void PluginsForm::on_mButtonSourceUpdateAll_clicked()
{
	for( int i = 0 ; i < ui->mSourceList->count() ; i++ )
	{
		QString RepoName = ui->mSourceList->item( i )->text();

		mPluginCache.updateRepo( RepoName );
	}
}


void PluginsForm::on_mButtonSourceRemove_clicked()
{
	QListWidgetItem		*item = ui->mSourceList->currentItem();

	if( QMessageBox::question( this, tr( "Remove Repository?" ), tr( "Are you sure you want to remove this repository?" ) ) == QMessageBox::No )
	{
		return;
	}

	mPluginCache.removeRepo( item->text() );

	rebuildPluginInformation();

	on_mSourceList_itemSelectionChanged();
}


void PluginsForm::on_mButtonSourceUpdate_clicked()
{
	QListWidgetItem		*item = ui->mSourceList->currentItem();

	mPluginCache.updateRepo( item->text() );
}

void PluginsForm::rebuildPluginInformation()
{
	SettingsHelper	Helper;
	PluginConfig	PC( Helper );

	ui->mSourceList->clear();
	ui->mPluginTable->clearContents();

	int row = 0;

	for( QString &RepoName : mPluginCache.repoNames() )
	{
		ui->mSourceList->addItem( RepoName );

		PluginRepoManifest	RM( mPluginCache.repoCacheDirectory().filePath( mPluginCache.repoManifestFilename( RepoName ) ), "win64" );

		for( QString &PluginName : RM.pluginList() )
		{
			int col = 0;

			ui->mPluginTable->insertRow( row );

			QCheckBox			*PluginEnabled = new QCheckBox();

			PluginEnabled->setStyleSheet( "text-align: center; margin-left:50%; margin-right:50%;" );

			ui->mPluginTable->setCellWidget( row, col++, PluginEnabled );

			QTableWidgetItem	*NameItem = new QTableWidgetItem( PluginName );

			ui->mPluginTable->setItem( row, col++, NameItem );

			QTableWidgetItem	*VersionItem = new QTableWidgetItem( RM.pluginLatestVersion( PluginName ).toString() );

			ui->mPluginTable->setItem( row, col++, VersionItem );

			QComboBox			*VersionList = new QComboBox();

			VersionList->addItem( tr( "None" ) );

			for( QVersionNumber &Version : mPluginCache.pluginVersions( PluginName ) )
			{
				VersionList->addItem( Version.toString() );
			}

			VersionList->setCurrentText( PC.installedPluginVersion( PluginName ).toString() );

			ui->mPluginTable->setCellWidget( row, col++, VersionList );

			row++;
		}
	}

	ui->mPluginTable->resizeColumnsToContents();
}

void PluginsForm::on_mButtonSourceAddFile_clicked()
{
	QString ArgDat = QFileDialog::getOpenFileName( this, tr( "Add Plugin Source" ) );

	if( ArgDat.isEmpty() )
	{
		return;
	}

	QUrl            repoUrl;
	QString         repoManifestFilename;
	bool            repoManifestRemove = false;
	QDateTime       repoModified;

	QFileInfo repoFileInfo( ArgDat );

	repoUrl = QUrl::fromLocalFile( repoFileInfo.absoluteFilePath() );

	qDebug() << repoUrl;

	repoManifestFilename = repoFileInfo.absoluteFilePath();

	repoModified = repoFileInfo.lastModified().toUTC();

	if( !repoManifestFilename.isEmpty() )
	{
		PluginRepoManifest      RepoManifest( repoManifestFilename, "win64" );

		RepoManifest.setModified( repoModified );

		mPluginCache.addRepoManifest( RepoManifest, repoUrl );

		if( repoManifestRemove )
		{
			QFile::remove( repoManifestFilename );
		}

		rebuildPluginInformation();
	}

	on_mSourceList_itemSelectionChanged();
}


void PluginsForm::on_mButtonSourceAddUrl_clicked()
{
	bool	ok;

	QString ArgDat = QInputDialog::getText( this, tr( "Add Plugin Source" ), tr( "URL:" ), QLineEdit::Normal, QString(), &ok );

	if( !ok || ArgDat.isEmpty() )
	{
		return;
	}

	QUrl            repoUrl;
	QString         repoManifestFilename;
	bool            repoManifestRemove = false;
	QDateTime       repoModified;

	repoUrl = QUrl( ArgDat );

	qDebug() << repoUrl;

	PluginActionDownload    RepoDown( repoUrl );

	RepoDown.setAutoRemove( false );

	if( RepoDown.action() )
	{
		repoManifestFilename = RepoDown.tempFileName();

		repoModified = RepoDown.modified();

		repoManifestRemove = true;
	}

	if( !repoManifestFilename.isEmpty() )
	{
		PluginRepoManifest      RepoManifest( repoManifestFilename, "win64" );

		RepoManifest.setModified( repoModified );

		mPluginCache.addRepoManifest( RepoManifest, repoUrl );

		if( repoManifestRemove )
		{
			QFile::remove( repoManifestFilename );
		}

		rebuildPluginInformation();
	}

	on_mSourceList_itemSelectionChanged();
}

/*!
 * \brief PluginsForm::on_mButtonApply_clicked
 */

void PluginsForm::on_mButtonApply_clicked()
{
	SettingsHelper	Helper;
	PluginConfig	Config( Helper );

	QMap<QString,QVersionNumber>	CacheDownloadList;
	QMap<QString,QVersionNumber>	InstallList;
	QStringList		RemoveList;

	for( int row = 0 ; row < ui->mPluginTable->rowCount() ; row++ )
	{
		QTableWidgetItem	*NameItem = ui->mPluginTable->item( row, 1 );
		QComboBox			*VersionComboBox = qobject_cast<QComboBox *>( ui->mPluginTable->cellWidget( row, 3 ) );

		QString				 PluginName = NameItem->text();
		QString				 VersionText = VersionComboBox->currentText();

		qDebug() << PluginName << VersionText;

		QVersionNumber      RequiredVersion = QVersionNumber::fromString( VersionText );
		QVersionNumber      InstalledVersion = Config.installedPluginVersion( PluginName );

		if( RequiredVersion == InstalledVersion )
		{
			continue;
		}

		if( !InstalledVersion.isNull() )
		{
			RemoveList << PluginName;
		}

		if( RequiredVersion.isNull() )
		{
			continue;
		}

		QString     PluginArchive = mPluginCache.cachedPluginFilename( PluginName, RequiredVersion );

		if( PluginArchive.isEmpty() )
		{
			CacheDownloadList.insert( PluginName, RequiredVersion );
		}

		InstallList.insert( PluginName, RequiredVersion );
	}

	for( auto it = CacheDownloadList.begin() ; it != CacheDownloadList.end() ; it++ )
	{
		QString	  PluginName = it.key();
		QVersionNumber	PluginVersion = CacheDownloadList.value( PluginName );

		QString   pluginFilename;
		bool      pluginRemove = false;
		QUrl      PluginUrl = mPluginCache.pluginUrl( PluginName, PluginVersion );

		if( !PluginUrl.isLocalFile() )
		{
			PluginActionDownload    pluginDown( PluginUrl );

			pluginDown.setAutoRemove( false );

			if( pluginDown.action() )
			{
				pluginFilename = pluginDown.tempFileName();

				pluginRemove = true;
			}
		}
		else
		{
			pluginFilename = QDir::toNativeSeparators( PluginUrl.toLocalFile() );
		}

		mPluginCache.addPluginToCache( PluginName, PluginVersion, pluginFilename );

		if( pluginRemove )
		{
			QFile::remove( pluginFilename );
		}
	}

	Helper.remove( "plugin-update" );

	Helper.beginGroup( "plugin-update" );

	Helper.beginWriteArray( "remove" );

	int ArrayIndex = 0;

	for( QString &RemovePlugin : RemoveList )
	{
		Helper.setArrayIndex( ArrayIndex++ );

		Helper.setValue( "plugin", RemovePlugin );
	}

	Helper.endArray();

	Helper.beginWriteArray( "install" );

	ArrayIndex = 0;

	for( auto it = InstallList.begin() ; it != InstallList.end() ; it++ )
	{
		QString	  PluginName = it.key();
		QVersionNumber	PluginVersion = InstallList.value( PluginName );

		Helper.setArrayIndex( ArrayIndex++ );

		Helper.setValue( "plugin", PluginName );
		Helper.setValue( "version", PluginVersion.toString() );
	}

	Helper.endArray();

	Helper.endGroup();

/*
	int					row = ui->mPluginTable->currentRow();

	QTableWidgetItem	*item = ui->mPluginTable->item( row, 1 );

	if( item )
	{
		PluginArchive = mPluginCache.cachedPluginFilename( ArgDat, PluginVersion );

		if( !PluginArchive.isEmpty() )
		{
			PluginActionInstall     PluginInstall( PluginArchive, App::dataDirectory().absolutePath() );

			if( PluginInstall.action() )
			{
				Config.setInstalledPluginVersion( ArgDat, PluginVersion );
			}
		}
	}
*/

	if( !RemoveList.isEmpty() || !InstallList.isEmpty() )
	{
		if( gApp->mainWindow()->close() )
		{
			gApp->setAppRestart( true );
		}
	}
}

