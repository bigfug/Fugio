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
 *
 * The user has clicked Apply so we will process the plugin changes they have specified
 */

void PluginsForm::on_mButtonApply_clicked()
{
	SettingsHelper	Helper;
	PluginConfig	Config( Helper );

	// Lets go through all the rows of the table and detect what changes the user made
	// to the current plugin configuration

	QMap<QString,QVersionNumber>	CacheDownloadList;
	QMap<QString,QVersionNumber>	InstallList;
	QStringList		RemoveList;

	for( int row = 0 ; row < ui->mPluginTable->rowCount() ; row++ )
	{
		QTableWidgetItem	*NameItem = ui->mPluginTable->item( row, 0 );

		if( !NameItem )
		{
			continue;
		}

		QComboBox			*VersionComboBox = qobject_cast<QComboBox *>( ui->mPluginTable->cellWidget( row, 2 ) );

		QString				 PluginName = NameItem->text();
		QString				 VersionText = VersionComboBox->currentText();

		QVersionNumber      RequiredVersion = QVersionNumber::fromString( VersionText );
		QVersionNumber      InstalledVersion = Config.installedPluginVersion( PluginName );

		// if the required version is the installed version (including if both are NULL) then continue

		if( RequiredVersion == InstalledVersion )
		{
			continue;
		}

		// if we have an already installed verion then remove it

		if( !InstalledVersion.isNull() )
		{
			RemoveList << PluginName;
		}

		if( RequiredVersion.isNull() )
		{
			continue;
		}

		// at this point, we want to install - check if the cache contains it and add
		// it to the download list if not

		QString     PluginArchive = mPluginCache.cachedPluginFilename( PluginName, RequiredVersion );

		if( PluginArchive.isEmpty() )
		{
			CacheDownloadList.insert( PluginName, RequiredVersion );
		}

		InstallList.insert( PluginName, RequiredVersion );
	}

	// before we can install new plugins, we have to make sure they are in the global cache
	// we can do this here

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

	// TODO: at this point we should check if the configuration is valid

	// we can't remove or install plugins now, as at this point they're loaded into memory
	// write out the remove/install data to the local config file to be acted on when
	// the app next starts (from main.cpp)

	Helper.remove( "plugin-update" );

	Helper.beginGroup( "plugin-update" );

	// write out the remove list

	Helper.beginWriteArray( "remove" );

	int ArrayIndex = 0;

	for( QString &RemovePlugin : RemoveList )
	{
		Helper.setArrayIndex( ArrayIndex++ );

		Helper.setValue( "plugin", RemovePlugin );
	}

	Helper.endArray();

	// write out the install list, with desied version numbers

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

	// Set the app restart flag if we need to remove or install any plugins

	if( !RemoveList.isEmpty() || !InstallList.isEmpty() )
	{
		if( gApp->mainWindow()->close() )
		{
			gApp->setAppRestart( true );
		}
	}
}

