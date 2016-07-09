#include "filewatchernode.h"

#include <QSettings>
#include <QPushButton>
#include <QDateTime>
#include <QDir>

#include "fugio/global_interface.h"
#include "fugio/context_interface.h"

FileWatcherNode::FileWatcherNode( QSharedPointer<fugio::NodeInterface> pNode ) :
	NodeControlBase( pNode ), mProcess( this )
{
	mPinStringInterface = pinOutput<fugio::VariantInterface *>( "String", mPinString, PID_STRING );

	connect( &mWatcher, SIGNAL(fileChanged(QString)), this, SLOT(onFileUpdate(QString)) );
}

FileWatcherNode::~FileWatcherNode( void )
{
}

QWidget *FileWatcherNode::gui()
{
	QPushButton		*GUI = new QPushButton( "Edit..." );

	connect( GUI, SIGNAL(clicked()), this, SLOT(onEditClicked()) );

	return( GUI );
}

void FileWatcherNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	pinUpdated( mPinString );
}

bool FileWatcherNode::initialise( void )
{
	return( true );
}

void FileWatcherNode::loadSettings(QSettings &pSettings)
{
	mPinStringInterface->setVariant( QString::fromUtf8( pSettings.value( "value" ).toByteArray() ) );
}

void FileWatcherNode::saveSettings( QSettings &pSettings ) const
{
	pSettings.setValue( "value", mPinStringInterface->variant() );
}

void FileWatcherNode::onEditClicked( void )
{
	if( mProcess.state() != QProcess::NotRunning )
	{
		return;
	}

	QString			TempFileName = mTempFile.fileName();

	if( TempFileName.isEmpty() )
	{
		mTempFile.setFileTemplate( QDir::tempPath() + "/" + mNode->name() );
		mTempFile.setAutoRemove( true );

		if( mTempFile.open() )
		{
			mTempFile.write( mPinStringInterface->variant().toString().toUtf8() );

			mTempFile.close();
		}

		TempFileName = mTempFile.fileName();

		mWatcher.addPath( TempFileName );
	}

#if defined(Q_OS_WIN)
	QString			Program = "C:/Program Files (x86)/Notepad++/notepad++.exe";
#elif defined(Q_OS_LINUX)
	QString			Program = "/usr/bin/gedit";
#elif defined(Q_OS_MAC)
	QString			Program = "/usr/bin/vi";
#endif

	QStringList		Args;

	Args.append( TempFileName );

	mProcess.start( Program, Args );
}

void FileWatcherNode::onFileUpdate( QString pFileName )
{
	QFile		FileRead( pFileName );

	if( FileRead.open( QFile::ReadOnly ) )
	{
		mPinStringInterface->setVariant( QString( FileRead.readAll() ) );

		FileRead.close();

		pinUpdated( mPinString );
	}
}
