#include "patchpromptdialog.h"
#include "ui_patchpromptdialog.h"
#include "mainwindow.h"
#include "app.h"
#include "undo/cmdnewcontext.h"

PatchPromptDialog::PatchPromptDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::PatchPromptDialog)
{
	ui->setupUi(this);

	QSettings			Settings;

	Settings.beginReadArray( "recent" );

	QStringList			RecentFiles;

	for( int i = 0 ; i < 10 ; i++ )
	{
		Settings.setArrayIndex( i );

		QString			RecentFile = Settings.value( "filename" ).toString();

		if( RecentFile.isEmpty() )
		{
			continue;
		}

		if( !QFile( RecentFile ).exists() )
		{
			continue;
		}

		RecentFiles << RecentFile;
	}

	if( !RecentFiles.isEmpty() )
	{
		ui->mRecentPatches->setEnabled( true );

		ui->mRecentPatches->clear();

		for( QString FileName : RecentFiles )
		{
			QListWidgetItem		*LWI = new QListWidgetItem();

			if( LWI )
			{
				QFileInfo			 FI( FileName );

				LWI->setText( FI.fileName() );
				LWI->setData( Qt::UserRole, FileName );
				LWI->setToolTip( FileName );

				ui->mRecentPatches->addItem( LWI );
			}
		}
	}
}

PatchPromptDialog::~PatchPromptDialog()
{
	delete ui;
}

void PatchPromptDialog::on_mButtonNewPatch_clicked()
{
	CmdNewContext		*Cmd = new CmdNewContext();

	gApp->undoGroup().activeStack()->push( Cmd );

	accept();
}

void PatchPromptDialog::on_mButtonOpenPatch_clicked()
{
	QString		FileName = gApp->mainWindow()->patchOpenDialog();

	if( !FileName.isEmpty() )
	{
		gApp->mainWindow()->loadPatch( FileName );

		gApp->mainWindow()->addFileToRecent( FileName );

		accept();
	}
}

void PatchPromptDialog::on_mButtonCancel_clicked()
{
	reject();
}

void PatchPromptDialog::on_mRecentPatches_itemDoubleClicked( QListWidgetItem *pItem )
{
	QString		FileName = pItem->data( Qt::UserRole ).toString();

	if( !FileName.isEmpty() )
	{
		gApp->mainWindow()->loadPatch( FileName );

		gApp->mainWindow()->addFileToRecent( FileName );

		accept();
	}
}
