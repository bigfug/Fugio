#include "snippitsform.h"
#include "ui_snippitsform.h"
#include <QStandardPaths>
#include <QMenu>
#include <QMessageBox>
#include <QDebug>
#include <QFile>
#include <QInputDialog>

SnippitsForm::SnippitsForm(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::SnippitsForm)
{
	ui->setupUi( this );

	QDir	UsrDir = QDir( QStandardPaths::writableLocation( QStandardPaths::DataLocation ) );

	if( !QDir( UsrDir.absoluteFilePath( "snippits" ) ).exists() )
	{
		UsrDir.mkdir( "snippits" );
	}

	UsrDir.cd( "snippits" );

	mUsrRoot = UsrDir.absolutePath();

	qInfo() << "Snippits Directory:" << mUsrRoot;

	mUserSystem.setRootPath( mUsrRoot );
	mUserSystem.setReadOnly( false );
	mUserSystem.setNameFilters( { "*.fug" } );

	ui->mUser->setDragDropMode( QAbstractItemView::InternalMove );
	ui->mUser->setDragEnabled( true );
	ui->mUser->setModel( &mUserSystem );
	ui->mUser->setRootIndex( mUserSystem.index( mUserSystem.rootPath() ) );
	ui->mUser->setSortingEnabled( true );
	ui->mUser->sortByColumn( 0, Qt::AscendingOrder );

	for( int i = 1 ; i < ui->mUser->header()->count() ; i++ )
	{
		ui->mUser->setColumnHidden( i, true );
	}

	ui->mUser->header()->hide();

	QDir	SysDir;

#if defined( Q_OS_MACX )
	SysDir = QDir( qApp->applicationDirPath() );

	SysDir.cdUp();
	SysDir.cdUp();
	SysDir.cdUp();

#if defined( QT_DEBUG )
	SysDir.cdUp();

	SysDir.cd( "Fugio" );
#endif

	SysDir.cd( "snippits" );
#endif

	qDebug() << "Fugio Snippits:" << SysDir.absolutePath();

	mFileSystem.setRootPath( SysDir.absolutePath() );
	mFileSystem.setReadOnly( true );
	mFileSystem.setNameFilters( { "*.fug" } );

	ui->mSystem->setDragDropMode( QAbstractItemView::DragOnly );
	ui->mSystem->setDragEnabled( true );
	ui->mSystem->setModel( &mFileSystem );
	ui->mSystem->setRootIndex( mFileSystem.index( mFileSystem.rootPath() ) );
	ui->mSystem->setSortingEnabled( true );
	ui->mSystem->sortByColumn( 0, Qt::AscendingOrder );

	for( int i = 1 ; i < ui->mSystem->header()->count() ; i++ )
	{
		ui->mSystem->setColumnHidden( i, true );
	}

	ui->mSystem->header()->hide();
}

SnippitsForm::~SnippitsForm()
{
	delete ui;
}

void SnippitsForm::on_mSnippetSource_currentIndexChanged( int index )
{
	ui->mSnippitStack->setCurrentIndex( index );
}

void SnippitsForm::on_mUser_customContextMenuRequested( const QPoint &pos )
{
	mUserPoint = pos;

	QMenu			 Menu;

	Menu.addAction( tr( "New folder..." ), this, SLOT(userNewFolder()) );

	QModelIndexList	 ItmLst = ui->mUser->selectionModel()->selectedIndexes();

	if( !ItmLst.isEmpty() )
	{
		Menu.addSeparator();

		Menu.addAction( tr( "Delete..." ), this, SLOT(userDeleteSelected()) );
	}

	Menu.exec( ui->mUser->mapToGlobal( pos ) );
}

void SnippitsForm::userDeleteSelected()
{
	if( QMessageBox::warning( 0, "Delete Snippits?", "Permanently delete these items (no undo)?", QMessageBox::No, QMessageBox::Yes ) == QMessageBox::Yes )
	{
		QModelIndexList	 ItmLst = ui->mUser->selectionModel()->selectedIndexes();

		for( QModelIndex ModIdx : ItmLst )
		{
			if( mUserSystem.isDir( ModIdx ) )
			{
				QDir	CurDir( mUserSystem.filePath( ModIdx ) );

				if( CurDir.exists() )
				{
					if( !CurDir.removeRecursively() )
					{
						qWarning() << tr( "Couldn't delete directory %1" ).arg( CurDir.absolutePath() );
					}
				}
			}
			else
			{
				QFile	CurFil( mUserSystem.filePath( ModIdx ) );

				if( CurFil.exists() )
				{
					if( !CurFil.remove() )
					{
						qWarning() << tr( "Couldn't delete file %1" ).arg( CurFil.fileName() );
					}
				}
			}
		}
	}
}

void SnippitsForm::userNewFolder()
{
	QString			DirNam = QInputDialog::getText( nullptr, tr( "New Folder" ), tr( "Enter a name for the new folder" ) );

	if( DirNam.isEmpty() )
	{
		return;
	}

	QModelIndex		UsrIdx = ui->mUser->indexAt( mUserPoint );

	if( UsrIdx.isValid() )
	{
		UsrIdx = ui->mUser->currentIndex();
	}

	QString			ParNam = ( UsrIdx.isValid() ? mUserSystem.filePath( UsrIdx ) : mUserSystem.rootPath() );
	QFileInfo		ParInf( ParNam );
	QDir			ParDir( ParNam );

	if( ParInf.isFile() )
	{
		ParDir.cdUp();
	}

	if( QFileInfo( ParDir.absoluteFilePath( DirNam ) ).exists() )
	{
		QMessageBox::warning( nullptr, tr( "Folder Exists" ), tr( "Can't make the folder - it already exists" ), QMessageBox::Ok, QMessageBox::NoButton );

		return;
	}

	if( !ParDir.mkdir( DirNam ) )
	{
		QMessageBox::warning( nullptr, tr( "Folder Error" ), tr( "Can't create the folder" ), QMessageBox::Ok, QMessageBox::NoButton );
	}
}
