#include "snippitsform.h"
#include "ui_snippitsform.h"
#include <QStandardPaths>

SnippitsForm::SnippitsForm(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::SnippitsForm)
{
	ui->setupUi( this );

	QDir	SnippetsDir = QDir( QStandardPaths::writableLocation( QStandardPaths::DataLocation ) );

	if( !QDir( SnippetsDir.absoluteFilePath( "snippets" ) ).exists() )
	{
		SnippetsDir.mkdir( "snippets" );
	}

	SnippetsDir.cd( "snippets" );

	qInfo() << "Snippets Directory:" << SnippetsDir.absolutePath();

	mSnippitRoot = SnippetsDir.absolutePath();

	mFileSystem.setRootPath( mSnippitRoot );
	//mFileSystem.setReadOnly( false );

	ui->mSnippits->setModel( &mFileSystem );

	ui->mSnippits->setRootIndex( mFileSystem.index( mSnippitRoot ) );

	ui->mSnippits->setDragDropMode( QAbstractItemView::DragDrop );
	ui->mSnippits->setDragEnabled( true );
}

SnippitsForm::~SnippitsForm()
{
	delete ui;
}

void SnippitsForm::on_mSnippits_doubleClicked(const QModelIndex &index)
{
	QFileInfo	FileInfo = mFileSystem.fileInfo( index );

	if( FileInfo.isDir() )
	{
		mFileSystem.setRootPath( FileInfo.path() );

		ui->mSnippits->setRootIndex( mFileSystem.index( FileInfo.path() ) );

		if( FileInfo.path() == QDir::currentPath() )
		{
			mFileSystem.setFilter( QDir::AllDirs | QDir::AllEntries | QDir::NoDotAndDotDot );
		}
		else
		{
			mFileSystem.setFilter( QDir::AllDirs | QDir::AllEntries | QDir::NoDot );
		}
	}
}
