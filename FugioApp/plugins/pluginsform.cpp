#include "pluginsform.h"
#include "ui_pluginsform.h"

#include <QInputDialog>

#include <globalprivate.h>
#include "app.h"

PluginsForm::PluginsForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PluginsForm)
{
    ui->setupUi(this);

	qDebug() << mPluginCache.pluginConfigFilename();

	PluginConfig	PC( mPluginCache.pluginConfigFilename() );

	int row = 0;

	for( QString RepoName : mPluginCache.repoNames() )
	{
		ui->mSourceList->addItem( RepoName );

		PluginRepoManifest	RM( mPluginCache.repoCacheDirectory().filePath( mPluginCache.repoManifestFilename( RepoName ) ), "win64" );

		for( QString PluginName : RM.pluginList() )
		{
			ui->mPluginTable->insertRow( row );

			QTableWidgetItem	*NameItem = new QTableWidgetItem( PluginName );

			ui->mPluginTable->setItem( row, 0, NameItem );

			QTableWidgetItem	*VersionItem = new QTableWidgetItem( RM.pluginLatestVersion( PluginName ).toString() );

			ui->mPluginTable->setItem( row, 1, VersionItem );

			QTableWidgetItem	*InstalledVersionItem = new QTableWidgetItem( PC.installedPluginVersion( PluginName ).toString() );

			ui->mPluginTable->setItem( row, 2, InstalledVersionItem );

			row++;
		}
	}
}

PluginsForm::~PluginsForm()
{
    delete ui;
}

void PluginsForm::on_mButtonSourceAdd_clicked()
{
	bool	ok;

	QString ArgDat = QInputDialog::getText( this, tr( "Add Plugin Source" ), tr( "Path or URL:" ), QLineEdit::Normal, QString(), &ok );

	if( !ok || ArgDat.isEmpty() )
	{
		return;
	}

	QUrl            repoUrl;
	QString         repoManifestFilename;
	bool            repoManifestRemove = false;
	QDateTime       repoModified;

	if( !QFileInfo( ArgDat ).isFile() )
	{
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
	}
	else
	{
		QFileInfo repoFileInfo( ArgDat );

		repoUrl = QUrl::fromLocalFile( repoFileInfo.absoluteFilePath() );

		qDebug() << repoUrl;

		repoManifestFilename = repoFileInfo.absoluteFilePath();

		repoModified = repoFileInfo.lastModified().toUTC();
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

		ui->mSourceList->clear();

		for( QString Src : mPluginCache.repoNames() )
		{
			ui->mSourceList->addItem( Src );
		}
	}
}

