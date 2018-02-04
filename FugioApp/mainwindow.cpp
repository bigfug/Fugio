#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDateTime>
#include <QFileDialog>
#include <QInputDialog>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QWidget>
#include <QMessageBox>
#include <QMetaObject>
#include <QMetaClassInfo>
#include <QDesktopServices>
#include <QColorDialog>
#include <QStyle>
#include <QThread>
#include <QProcess>
#include <QScrollBar>

#include "nodeprivate.h"

#include "app.h"

#include <QPainterPath>

#include "nodeitem.h"
#include "contextprivate.h"
#include "contextwidgetprivate.h"
#include "contextsubwindow.h"

#include "undo/cmdnewcontext.h"
#include "undo/cmdnodeadd.h"
#include "undo/cmdsetduration.h"
#include "undo/cmdnoteadd.h"

#include <fugio/global_interface.h>
#include <fugio/context_interface.h>
#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>

#include <fugio/utils.h>

#include <QFile>
#include <QTextStream>

#include "stylesheetform.h"
#include "contextview.h"

#include "patchpromptdialog.h"

#include "settings/settingsdialog.h"

#if QT_VERSION < QT_VERSION_CHECK( 5, 5, 0 )
#define qInfo qDebug
#endif

extern void log_file( const QString &pLogDat );

void MainWindow::logger_static( QtMsgType type, const QMessageLogContext &context, const QString &msg )
{
	qobject_cast<MainWindow *>( qobject_cast<App *>( qApp )->mainWindow() )->logger( type, context, msg );
}

void MainWindow::logger( QtMsgType type, const QMessageLogContext &context, const QString &msg )
{
	const bool isGuiThread = QThread::currentThread() == QCoreApplication::instance()->thread();

	Q_UNUSED( context )

	QByteArray localMsg = msg.toLocal8Bit();

	QString		LogDat;

	LogDat.append( QDateTime::currentDateTime().toString( Qt::ISODate ) );
	LogDat.append( ": " );
	LogDat.append( localMsg );

	switch (type)
	{
		case QtDebugMsg:
			fprintf(stderr, "DBUG: %s\n", localMsg.constData() );
			break;
#if ( QT_VERSION >= QT_VERSION_CHECK( 5, 5, 0 ) )
		case QtInfoMsg:
			fprintf(stderr, "INFO: %s\n", localMsg.constData() );
			break;
#endif
		case QtWarningMsg:
			fprintf(stderr, "WARN: %s\n", localMsg.constData() );
			break;
		case QtCriticalMsg:
			fprintf(stderr, "CRIT: %s\n", localMsg.constData() );
			break;
		case QtFatalMsg:
			fprintf(stderr, "FATL: %s\n", localMsg.constData() );
			break;
	}

	fflush( stderr );

	log_file( LogDat );

	if( isGuiThread )
	{
		ui->mLogger->appendPlainText( msg );
	}
	else
	{
		emit log( msg );
	}

	if( type == QtFatalMsg )
	{
		abort();
	}
}

bool MainWindow::addExamplesPath( const QString &pPath )
{
	if( QFileInfo( pPath ).exists() )
	{
		qInfo() << "Loading examples:" << pPath;

		populateExamples( pPath, ui->menuExamples );

		return( true );
	}

	return( false );
}

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow), mUndoStack( &gApp->undoGroup() ), mEditTarget( nullptr )
{
	ui->setupUi( this );

	gApp->global().registerInterface( IID_EDITOR, this );

	//connect( gApp->global().qobject(), SIGNAL(editTargetChanged(fugio::EditInterface*)), this, SLOT(onEditTarget(fugio::EditInterface*)) );

	ui->actionExit->setShortcut( QKeySequence::Quit );

	mFileSystem.setRootPath( QDir::currentPath() );

	ui->mTreeView->setModel( &mFileSystem );

	ui->mTreeView->setRootIndex( mFileSystem.index( "G:\\clips" ) );

	if( ( mActionRedo = gApp->undoGroup().createRedoAction( this ) ) != 0 )
	{
		mActionRedo->setShortcut( QKeySequence::Redo );

		ui->menu_Edit->insertAction( ui->menu_Edit->actions().at( 0 ), mActionRedo );

		//connect( &gApp->undoGroup(), SIGNAL(canRedoChanged(bool)), mActionRedo, SLOT(setEnabled(bool)) );
	}

	if( ( mActionUndo = gApp->undoGroup().createUndoAction( this ) ) != 0 )
	{
		mActionUndo->setShortcut( QKeySequence::Undo );

		mActionUndo->setIcon( QIcon::fromTheme( "edit-undo" ) );

		ui->menu_Edit->insertAction( ui->menu_Edit->actions().at( 0 ), mActionUndo );

		//connect( &gApp->undoGroup(), SIGNAL(canUndoChanged(bool)), mActionUndo, SLOT(setEnabled(bool)) );
	}

	mUndoStack.setActive();

	ui->actionCut->setShortcut( QKeySequence::Cut );
	ui->actionCopy->setShortcut( QKeySequence::Copy );
	ui->actionPaste->setShortcut( QKeySequence::Paste );

	connect( gApp->global().qobject(), SIGNAL(contextAdded(QSharedPointer<fugio::ContextInterface>)), this, SLOT(contextAdded(QSharedPointer<fugio::ContextInterface>)) );
	connect( gApp->global().qobject(), SIGNAL(contextRemoved(QSharedPointer<fugio::ContextInterface>)), this, SLOT(contextRemoved(QSharedPointer<fugio::ContextInterface>)) );

	connect( &gApp->undoGroup(), SIGNAL(cleanChanged(bool)), this, SLOT(onCleanChanged(bool)) );

	connect( gApp->global().qobject(), SIGNAL(fps(qreal)), this, SLOT(setFPS(qreal)) );

	updateWindowTitle();

	updateRecentFileList();

	connect( ui->actionRecent1, &QAction::triggered, this, &MainWindow::recentFileActivated );
	connect( ui->actionRecent2, &QAction::triggered, this, &MainWindow::recentFileActivated );
	connect( ui->actionRecent3, &QAction::triggered, this, &MainWindow::recentFileActivated );
	connect( ui->actionRecent4, &QAction::triggered, this, &MainWindow::recentFileActivated );
	connect( ui->actionRecent5, &QAction::triggered, this, &MainWindow::recentFileActivated );
	connect( ui->actionRecent6, &QAction::triggered, this, &MainWindow::recentFileActivated );
	connect( ui->actionRecent7, &QAction::triggered, this, &MainWindow::recentFileActivated );
	connect( ui->actionRecent8, &QAction::triggered, this, &MainWindow::recentFileActivated );
	connect( ui->actionRecent9, &QAction::triggered, this, &MainWindow::recentFileActivated );
	connect( ui->actionRecent0, &QAction::triggered, this, &MainWindow::recentFileActivated );

	connect( ui->menu_Window, SIGNAL(aboutToShow()), this, SLOT(buildWindowMenu()) );

	//-------------------------------------------------------------------------

	connect( &mActiveWindowMapper, SIGNAL(mapped(QWidget*)), this, SLOT(setActiveWindow(QWidget*)) );

	linkWindowVisibilitySignal( ui->actionLogger, ui->mDockWidthLogger );
	linkWindowVisibilitySignal( ui->actionBrowser, ui->mDockWidgetBrowser );
	linkWindowVisibilitySignal( ui->actionNodes, ui->mNodeDockWidget );
	linkWindowVisibilitySignal( ui->actionPerformance, ui->mPerformanceDockWidget );
	linkWindowVisibilitySignal( ui->actionSnippets, ui->mSnippetsDockWidget );
	linkWindowVisibilitySignal( ui->actionStylesheet, ui->mStyleSheetDockWidget );

	connect( ui->actionTile, SIGNAL(triggered()), ui->mWorkArea, SLOT(tileSubWindows()) );
	connect( ui->actionCascade, SIGNAL(triggered()), ui->mWorkArea, SLOT(cascadeSubWindows()) );

	//-------------------------------------------------------------------------

	addExamplesPath( gApp->global().sharedDataPath() + "/examples" );

	//-------------------------------------------------------------------------

	ui->actionRescan->setVisible( false );

	//-------------------------------------------------------------------------

//	show();

//	QTimer::singleShot( 500, ui->mStyleSheet, SLOT(stylesApply()) );

	//-------------------------------------------------------------------------
	// doesn't do what you might hope...

	QDesktopServices::setUrlHandler( "fugio", this, "fugioUrl" );

	//-------------------------------------------------------------------------

	QSettings		Settings;

	restoreGeometry( Settings.value( "geometry", saveGeometry() ).toByteArray() );
	restoreState( Settings.value( "state", saveState() ).toByteArray() );

	//-------------------------------------------------------------------------

	timeout();

	mMessageHandler = qInstallMessageHandler( MainWindow::logger_static );

	connect( this, SIGNAL(log(QString)), ui->mLogger, SLOT(appendHtml(QString)) );

	// Add a custom context menu to the Logger Window

	ui->mLogger->setContextMenuPolicy( Qt::CustomContextMenu );

	connect( ui->mLogger, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(loggerContextMenu(QPoint)) );
}

MainWindow::~MainWindow( void )
{
	QSettings		Settings;

	Settings.setValue( "geometry", saveGeometry() );
	Settings.setValue( "state", saveState() );

	disconnect( &gApp->undoGroup() );

	gApp->undoGroup().removeStack( &mUndoStack );

	gApp->global().unregisterInterface( IID_EDITOR );

	delete ui;
}

void MainWindow::createDeviceMenu()
{
	QStringList		DevLst = gApp->global().deviceFactoryMenuTextList();

	DevLst.sort();

	for( QString S : DevLst )
	{
		ui->menuDevices->addAction( QString( "%1..." ).arg( S ), this, SLOT(onDeviceMenu()) );
	}

	if( DevLst.isEmpty() )
	{
		ui->menuDevices->setHidden( true );
	}
}

void MainWindow::setFPS( qreal FPS )
{
	QString				 Status = QString( "FPS: %1" ).arg( FPS );

//	InterfaceOpenGL		*GL = qobject_cast<InterfaceOpenGL *>( gApp->global().findInterface( IID_OPENGL ) );

//	if( GL )
//	{
//		Status += QString( " - TPS: %1" ).arg( GL->triangleCount() );

//		GL->resetTriangleCount();
//	}

	ui->mStatusBar->showMessage( Status );
}

void MainWindow::timeout()
{
	for( QMdiSubWindow *SubWin : ui->mWorkArea->subWindowList() )
	{
		ContextSubWindow		*SW = qobject_cast<ContextSubWindow *>( SubWin );

		if( SW == 0 )
		{
			continue;
		}

		ContextWidgetPrivate	*CW = SW->contextWidget();

		if( CW == 0 )
		{
			continue;
		}

		CW->timeout();
	}

	QTimer::singleShot( 40, this, SLOT(timeout()) );
}

void MainWindow::closeEvent( QCloseEvent *pEvent )
{
	foreach( QSharedPointer<fugio::ContextInterface> Context, gApp->global().contexts() )
	{
		ContextSubWindow	*SW = findContextWindow( Context );

		if( SW != 0 )
		{
			ContextWidgetPrivate	*CW = SW->contextWidget();

			if( !CW->undoStack()->isClean() )
			{
				ui->mWorkArea->setActiveSubWindow( SW );

				if( !SW->close() )
				{
					pEvent->ignore();

					return;
				}
			}
		}
	}

	qInstallMessageHandler( mMessageHandler );

	QApplication::quit();
}

ContextSubWindow *MainWindow::findContextWindow( QSharedPointer<fugio::ContextInterface> pContext )
{
	for( QMdiSubWindow *SubWin : ui->mWorkArea->subWindowList() )
	{
		ContextSubWindow		*SW = qobject_cast<ContextSubWindow *>( SubWin );

		if( !SW )
		{
			continue;
		}

		ContextWidgetPrivate	*CW = SW->contextWidget();

		if( CW == 0 )
		{
			continue;
		}

		if( CW->context() != pContext )
		{
			continue;
		}

		return( SW );
	}

	return( 0 );
}

QStringList MainWindow::patchOpenDialog()
{
	const QString		DatDir = QStandardPaths::writableLocation( QStandardPaths::DocumentsLocation );

	QSettings			Settings;

	QString				PatchDirectory = Settings.value( "patch-directory", QDir( DatDir ).absoluteFilePath( "Fugio" ) ).toString();

	QStringList			FileList = QFileDialog::getOpenFileNames( this, tr( "Open Patch" ), PatchDirectory, tr( "Fugio Patches (*.fug)" ) );

	if( !FileList.isEmpty() )
	{
		QString		FirstFileName = FileList.first();

		if( QFile( FirstFileName ).exists() )
		{
			PatchDirectory = QFileInfo( FirstFileName ).absoluteDir().path();

			Settings.setValue( "patch-directory", PatchDirectory );
		}
	}

	return( FileList );
}

void MainWindow::stylesApply()
{
	ui->mStyleSheet->stylesApply();
}

void MainWindow::on_actionOpen_triggered()
{
	QStringList		FileList = patchOpenDialog();

	for( const QString &FileName : FileList )
	{
		loadPatch( FileName );

		addFileToRecent( FileName );
	}
}

void MainWindow::on_actionSave_triggered()
{
	if( ui->mWorkArea->currentSubWindow() )
	{
		ContextWidgetPrivate		*CV = qobject_cast<ContextWidgetPrivate *>( ui->mWorkArea->currentSubWindow()->widget() );

		if( CV )
		{
			CV->userSave();

			ui->mWorkArea->currentSubWindow()->setWindowTitle( CV->filename() );
		}
	}
}

void MainWindow::contextAdded( QSharedPointer<fugio::ContextInterface> pContext )
{
	ContextWidgetPrivate	*CV = new ContextWidgetPrivate();

	if( CV == 0 )
	{
		return;
	}

	CV->setContext( pContext );

	ContextSubWindow *subWindow1 = new ContextSubWindow();

	subWindow1->setWidget( CV );

	subWindow1->setAttribute(Qt::WA_DeleteOnClose);

	ui->mWorkArea->addSubWindow(subWindow1);

	CV->showMaximized();

	connect( CV, SIGNAL(contextFilenameChanged(QString)), this, SLOT(addFileToRecent(QString)) );

	QTimer::singleShot( 500, ui->mStyleSheet, SLOT(stylesApply()) );
}

void MainWindow::contextRemoved( QSharedPointer<fugio::ContextInterface> pContext )
{
	Q_UNUSED( pContext )
}

void MainWindow::on_actionSaveAs_triggered()
{
	if( ui->mWorkArea->currentSubWindow() == 0 )
	{
		return;
	}

	ContextWidgetPrivate		*CV = qobject_cast<ContextWidgetPrivate *>( ui->mWorkArea->currentSubWindow()->widget() );

	if( CV == 0 )
	{
		return;
	}

	CV->userSaveAs();
}

void MainWindow::on_actionNew_triggered()
{
	CmdNewContext		*Cmd = new CmdNewContext();

	mUndoStack.push( Cmd );
}

QSharedPointer<fugio::ContextInterface> MainWindow::currentContext( void )
{
	ContextSubWindow	*SubWin = qobject_cast<ContextSubWindow *>( ui->mWorkArea->currentSubWindow() );

	if( SubWin )
	{
		if( ContextWidgetPrivate *CW = SubWin->contextWidget() )
		{
			return( CW->context() );
		}
	}

	return( QSharedPointer<fugio::ContextInterface>() );
}

void MainWindow::on_mWorkArea_subWindowActivated( QMdiSubWindow *pSubWin )
{
	ContextSubWindow	*SubWin = qobject_cast<ContextSubWindow *>( pSubWin );

	if( !SubWin )
	{
		mUndoStack.setActive();

		ui->actionSave->setEnabled( false );
		ui->actionSaveAs->setEnabled( false );
	}
	else if( ContextWidgetPrivate *CW = SubWin->contextWidget() )
	{
		CW->undoStack()->setActive();

		ui->actionSave->setEnabled( true );
		ui->actionSaveAs->setEnabled( true );
	}
}

void MainWindow::on_actionCursorToStart_triggered()
{
	if( ui->mWorkArea->currentSubWindow() )
	{
		ContextWidgetPrivate		*CV = qobject_cast<ContextWidgetPrivate *>( ui->mWorkArea->currentSubWindow()->widget() );

		if( CV != 0 )
		{
			CV->context()->setPlayheadPosition( 0 );
		}
	}
}

void MainWindow::on_actionCursorToEnd_triggered()
{
	if( ui->mWorkArea->currentSubWindow() != 0 )
	{
		ContextWidgetPrivate		*CV = qobject_cast<ContextWidgetPrivate *>( ui->mWorkArea->currentSubWindow()->widget() );

		if( CV != 0 )
		{
			CV->context()->setPlayheadPosition( CV->context()->duration() );
		}
	}
}

void MainWindow::on_actionPlayToggle_triggered()
{
	if( ui->mWorkArea->currentSubWindow() != 0 )
	{
		ContextWidgetPrivate		*CV = qobject_cast<ContextWidgetPrivate *>( ui->mWorkArea->currentSubWindow()->widget() );

		if( CV != 0 )
		{
			if( CV->context()->state() == fugio::ContextInterface::Playing )
			{
				CV->context()->stop();
			}
			else
			{
				CV->context()->play();
			}
		}
	}
}

void MainWindow::onCleanChanged( bool pClean )
{
	Q_UNUSED( pClean )
}

void MainWindow::onDeviceMenu( void )
{
	QAction		*A = qobject_cast<QAction *>( sender() );

	QString		 S = A->text();

	S.chop( 3 );

	gApp->global().deviceFactoryGui( this, S );
}

void MainWindow::updateWindowTitle( void )
{
#if defined( QT_DEBUG)
	setWindowTitle( QString( tr( "%1 v%2 by Alex May - DEBUG VERSION (not for distribution)" ) ).arg( qApp->applicationName() ).arg( qApp->applicationVersion() ) );
#else
	setWindowTitle( QString( tr( "%1 v%2 by Alex May" ) ).arg( qApp->applicationName() ).arg( qApp->applicationVersion() ) );
#endif
}

void MainWindow::on_actionSet_duration_triggered()
{
	if( ui->mWorkArea->currentSubWindow() != 0 )
	{
		ContextWidgetPrivate		*CV = qobject_cast<ContextWidgetPrivate *>( ui->mWorkArea->currentSubWindow()->widget() );

		if( CV != 0 )
		{
			QString	NewTim = QInputDialog::getText( this, tr( "Set duration" ), tr( "Duration (Hours:Minutes:Seconds:Milliseconds):" ), QLineEdit::Normal, fugio::utils::timeToString( CV->context()->duration() ) );

			if( !NewTim.isEmpty() )
			{
				qreal	t = fugio::utils::stringToTime( NewTim );

				if( t != CV->context()->duration() && t > 0 )
				{
					CmdSetDuration		*Cmd = new CmdSetDuration( CV->context(), t );

					CV->undoStack()->push( Cmd );
				}
			}
		}
	}
}

void MainWindow::on_actionExit_triggered()
{
	close();
}

void MainWindow::on_mNodeList_itemActivated( QListWidgetItem *pItem )
{
	if( ui->mWorkArea->currentSubWindow() != 0 )
	{
		ContextWidgetPrivate		*CV = qobject_cast<ContextWidgetPrivate *>( ui->mWorkArea->currentSubWindow()->widget() );

		if( CV != 0 )
		{
			QString		NodeName = QInputDialog::getText( this, "Name", "Name", QLineEdit::Normal, pItem->text() );

			if( !NodeName.isEmpty() )
			{
				CmdNodeAdd		*Cmd = new CmdNodeAdd( CV, NodeName, pItem->data( Qt::UserRole ).toUuid() );

				CV->view()->setNodePositionFlag();

				CV->undoStack()->push( Cmd );
			}
		}
	}
}

void MainWindow::on_actionAdd_Note_triggered()
{
	if( ui->mWorkArea->currentSubWindow() != 0 )
	{
		ContextWidgetPrivate		*CV = qobject_cast<ContextWidgetPrivate *>( ui->mWorkArea->currentSubWindow()->widget() );

		if( CV != 0 )
		{
			CmdNoteAdd		*Cmd = new CmdNoteAdd( CV );

			CV->undoStack()->push( Cmd );
		}
	}
}

void MainWindow::on_actionAbout_Fugio_triggered()
{
	QFile	HTML( ":/html/about.html" );

	QString	 TxtLst;

	if( HTML.open( QFile::ReadOnly | QFile::Text ) )
	{
		TxtLst = HTML.readAll();

		HTML.close();
	}

	TxtLst.replace( "[VERSION]", QApplication::applicationVersion() );

	QMessageBox::about( this, "Fugio " + qApp->applicationVersion(), TxtLst );
}

void MainWindow::on_actionAbout_Qt_triggered()
{
	QMessageBox::aboutQt( this );
}

void MainWindow::on_actionOnline_Documentation_triggered()
{
	QDesktopServices::openUrl( QUrl( "http://wiki.bigfug.com/Fugio?utm_source=fugio&utm_medium=help" ) );
}

void MainWindow::on_actionSet_Colour_triggered()
{
	if( ui->mWorkArea->currentSubWindow() == 0 )
	{
		return;
	}

	ContextWidgetPrivate		*CV = qobject_cast<ContextWidgetPrivate *>( ui->mWorkArea->currentSubWindow()->widget() );

	if( CV == 0 )
	{
		return;
	}

	QColor C = QColorDialog::getColor( Qt::white, this );

	if( C.isValid() )
	{
		CV->view()->setSelectedColour( C );
	}
}

void MainWindow::on_actionCut_triggered()
{
	if( ui->mWorkArea->currentSubWindow() == 0 )
	{
		return;
	}

	ContextWidgetPrivate		*CV = qobject_cast<ContextWidgetPrivate *>( ui->mWorkArea->currentSubWindow()->widget() );

	if( CV == 0 )
	{
		return;
	}

	CV->cut();
}

void MainWindow::on_actionCopy_triggered()
{
	if( ui->mWorkArea->currentSubWindow() == 0 )
	{
		return;
	}

	ContextWidgetPrivate		*CV = qobject_cast<ContextWidgetPrivate *>( ui->mWorkArea->currentSubWindow()->widget() );

	if( CV == 0 )
	{
		return;
	}

	CV->copy();
}

void MainWindow::on_actionPaste_triggered()
{
	if( ui->mWorkArea->currentSubWindow() == 0 )
	{
		return;
	}

	ContextWidgetPrivate		*CV = qobject_cast<ContextWidgetPrivate *>( ui->mWorkArea->currentSubWindow()->widget() );

	if( CV == 0 )
	{
		return;
	}

	CV->paste();
}

void MainWindow::recentFileActivated( void )
{
	QAction			*A = qobject_cast<QAction *>( sender() );

	if( A )
	{
		QSharedPointer<fugio::ContextInterface>	 C = gApp->global().newContext();

		if( C == 0 )
		{
			return;
		}

		ContextSubWindow		*SubWin = findContextWindow( C );

		if( !SubWin )
		{
			return;
		}

		SubWin->contextWidget()->load( A->text() );

		addFileToRecent( A->text() );
	}
}

void MainWindow::addFileToRecent( const QString &pFileName )
{
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

	Settings.endArray();

	RecentFiles.removeAll( pFileName );

	RecentFiles.prepend( pFileName );

	Settings.beginWriteArray( "recent" );

	for( int i = 0 ; i < RecentFiles.size() ; i++ )
	{
		Settings.setArrayIndex( i );

		Settings.setValue( "filename", RecentFiles.at( i ) );
	}

	Settings.endArray();

	updateRecentFileList();
}

void MainWindow::updateRecentFileList( void )
{
	QSettings			Settings;

	Settings.beginReadArray( "recent" );

	QList<QAction *>	RecentActions;

	RecentActions << ui->actionRecent1;
	RecentActions << ui->actionRecent2;
	RecentActions << ui->actionRecent3;
	RecentActions << ui->actionRecent4;
	RecentActions << ui->actionRecent5;
	RecentActions << ui->actionRecent6;
	RecentActions << ui->actionRecent7;
	RecentActions << ui->actionRecent8;
	RecentActions << ui->actionRecent9;
	RecentActions << ui->actionRecent0;

	QStringList			RecentFiles;

	for( int i = 0 ; i < RecentActions.size() ; i++ )
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

	for( int i = 0 ; i < RecentActions.size() ; i++ )
	{
		if( i >= RecentFiles.size() )
		{
			RecentActions[ i ]->setVisible( false );
		}
		else
		{
			RecentActions[ i ]->setText( RecentFiles[ i ] );
		}
	}
}

void MainWindow::linkWindowVisibilitySignal( QAction *pAction, QDockWidget *pWidget )
{
	pAction->blockSignals( true );
	pWidget->blockSignals( true );

	connect( pWidget, SIGNAL(visibilityChanged(bool)), pAction, SLOT(setChecked(bool)) );

	connect( pAction, SIGNAL(triggered(bool)), pWidget, SLOT(setVisible(bool)) );

	pAction->blockSignals( false );
	pWidget->blockSignals( false );
}

void MainWindow::buildWindowMenu()
{
	ui->menu_Window->clear();

	ui->menu_Window->addAction( ui->actionBrowser );
	ui->menu_Window->addAction( ui->actionLogger );
	ui->menu_Window->addAction( ui->actionNodes );
	ui->menu_Window->addAction( ui->actionPerformance );
	ui->menu_Window->addAction( ui->actionSnippets );
	ui->menu_Window->addAction( ui->actionStylesheet );

	if( ui->mWorkArea->subWindowList().isEmpty() )
	{
		return;
	}

	ui->menu_Window->addSeparator();

	ui->menu_Window->addAction( ui->actionTile );
	ui->menu_Window->addAction( ui->actionCascade );

	ui->menu_Window->addSeparator();

	for( int i = 0 ; i < ui->mWorkArea->subWindowList().size() ; i++ )
	{
		ContextSubWindow *SubWin = qobject_cast<ContextSubWindow *>( ui->mWorkArea->subWindowList().at( i ) );

		if( !SubWin )
		{
			continue;
		}

		QString text;

		if (i < 9)
		{
			text = tr("&%1 %2").arg(i + 1)
				   .arg( SubWin->contextWidget()->filename() );
		}
		else
		{
			text = tr("%1 %2").arg(i + 1)
				   .arg( SubWin->contextWidget()->filename() );
		}

		QAction *action  = ui->menu_Window->addAction( text );
		action->setCheckable( true );
		action ->setChecked( SubWin == ui->mWorkArea->activeSubWindow() );
		connect(action, SIGNAL(triggered()), &mActiveWindowMapper, SLOT(map()));
		mActiveWindowMapper.setMapping( action, SubWin );
	}
}

void MainWindow::setActiveWindow( QWidget *pWidget )
{
	QMdiSubWindow	*SubWin = qobject_cast<QMdiSubWindow *>( pWidget );

	if( SubWin )
	{
		ui->mWorkArea->setActiveSubWindow( SubWin );
	}
}

void MainWindow::populateExamples( const QString &pDir, QMenu *pMenu )
{
	QDir		Dir( pDir );
	QStringList	Lst;

	Lst << "*.fug";

	for( QFileInfo FileInfo : Dir.entryInfoList( QStringList(), QDir::AllDirs | QDir::NoDotAndDotDot, QDir::Name ) )
	{
		QMenu	*Menu = pMenu->addMenu( FileInfo.baseName() );

		populateExamples( QDir( pDir ).absoluteFilePath( FileInfo.baseName() ), Menu );
	}

	for( QFileInfo FileInfo : Dir.entryInfoList( Lst, QDir::Files, QDir::Name ) )
	{
		QAction	*Action = pMenu->addAction( FileInfo.baseName(), this, SLOT(onExample()) );

		if( Action )
		{
			Action->setProperty( "filepath", FileInfo.absoluteFilePath() );
		}
	}
}

void MainWindow::onExample()
{
	QAction		*A = qobject_cast<QAction *>( sender() );

	if( !A )
	{
		return;
	}

	QString		 S = A->property( "filepath" ).toString();

	if( !QFileInfo( S ).exists() )
	{
		return;
	}

	QSharedPointer<fugio::ContextInterface>	 C = gApp->global().newContext();

	if( !C )
	{
		return;
	}

	ContextSubWindow		*SubWin = findContextWindow( C );

	if( !SubWin )
	{
		return;
	}

	SubWin->contextWidget()->load( S );
}

void MainWindow::onEditTarget( fugio::EditInterface *pEditTarget )
{
	ui->actionCut->disconnect();
	ui->actionCopy->disconnect();
	ui->actionPaste->disconnect();

	if( pEditTarget )
	{
		connect( ui->actionCut, SIGNAL(triggered()), dynamic_cast<QObject *>( pEditTarget ), SLOT(cut()) );
		connect( ui->actionCopy, SIGNAL(triggered()), dynamic_cast<QObject *>( pEditTarget ), SLOT(copy()) );
		connect( ui->actionPaste, SIGNAL(triggered()), dynamic_cast<QObject *>( pEditTarget ), SLOT(paste()) );

		ui->actionCut->setEnabled( true );
		ui->actionCopy->setEnabled( true );
		ui->actionPaste->setEnabled( true );
	}
	else
	{
		ui->actionCut->setEnabled( false );
		ui->actionCopy->setEnabled( false );
		ui->actionPaste->setEnabled( false );
	}
}

void MainWindow::on_actionContact_Developers_triggered()
{
	QDesktopServices::openUrl( QUrl( "http://www.bigfug.com/contact/?utm_source=fugio&utm_medium=contact" ) );
}

void MainWindow::on_actionFacebook_Page_triggered()
{
	QDesktopServices::openUrl( QUrl( "https://www.facebook.com/fugioapp" ) );
}

void MainWindow::on_actionFacebook_Users_Group_triggered()
{
	QDesktopServices::openUrl( QUrl( "https://www.facebook.com/groups/fugio/" ) );
}

void MainWindow::fugioUrl(const QUrl &pUrl)
{
	qDebug() << pUrl.toDisplayString();
}

void MainWindow::on_actionAppend_triggered()
{
	if( !ui->mWorkArea->currentSubWindow() )
	{
		return;
	}

	ContextWidgetPrivate	*CW = qobject_cast<ContextWidgetPrivate *>( ui->mWorkArea->currentSubWindow()->widget() );

	if( !CW )
	{
		return;
	}

	QSettings				 Settings;

	QString					 DatDir = QStandardPaths::writableLocation( QStandardPaths::DocumentsLocation );

	QString					 PatchDirectory = Settings.value( "patch-directory", QDir( DatDir ).absoluteFilePath( "Fugio" ) ).toString();

	QString		FileName = QFileDialog::getOpenFileName( this, tr( "Append Patch" ), PatchDirectory, tr( "Fugio Patches (*.fug)" ) );

	if( !QFile( FileName ).exists() )
	{
		return;
	}

	PatchDirectory = QFileInfo( FileName ).absoluteDir().path();

	Settings.setValue( "patch-directory", PatchDirectory );

	CW->context()->load( FileName, true );
}

void MainWindow::on_actionSelect_to_snippit_triggered()
{
	if( !ui->mWorkArea->currentSubWindow() )
	{
		return;
	}

	ContextWidgetPrivate		*CV = qobject_cast<ContextWidgetPrivate *>( ui->mWorkArea->currentSubWindow()->widget() );

	if( !CV )
	{
		return;
	}

	QString		FileName = QInputDialog::getText( this, tr( "Name Snippet" ), tr( "Enter the name for the new Snippet" ) );

	if( FileName.isEmpty() )
	{
		return;
	}

	QFileInfo	FileInfo( QDir( gApp->userSnippetsDirectory() ), FileName );

	if( FileInfo.suffix().toLower() != "fug" )
	{
		FileInfo.setFile( QDir( gApp->userSnippetsDirectory() ), FileName + ".fug" );
	}

	if( FileInfo.exists() )
	{
		if( QMessageBox::question( this, tr( "Snippet Exists" ), tr( "A snippit already exists with that name, do you want to overwrite it?" ), QMessageBox::Save | QMessageBox::Cancel ) != QMessageBox::Save )
		{
			return;
		}
	}

	CV->saveSelectedTo( FileInfo.absoluteFilePath() );
}

void MainWindow::on_actionGroup_triggered()
{
	if( ui->mWorkArea->currentSubWindow() == 0 )
	{
		return;
	}

	ContextWidgetPrivate		*CV = qobject_cast<ContextWidgetPrivate *>( ui->mWorkArea->currentSubWindow()->widget() );

	if( CV == 0 )
	{
		return;
	}

	CV->groupSelected();
}

void MainWindow::on_actionGroup_Parent_triggered()
{
	if( ui->mWorkArea->currentSubWindow() == 0 )
	{
		return;
	}

	ContextWidgetPrivate		*CV = qobject_cast<ContextWidgetPrivate *>( ui->mWorkArea->currentSubWindow()->widget() );

	if( CV == 0 )
	{
		return;
	}

	CV->groupParent();
}

void MainWindow::on_actionUngroup_triggered()
{
	if( ui->mWorkArea->currentSubWindow() == 0 )
	{
		return;
	}

	ContextWidgetPrivate		*CV = qobject_cast<ContextWidgetPrivate *>( ui->mWorkArea->currentSubWindow()->widget() );

	if( CV == 0 )
	{
		return;
	}

	CV->ungroupSelected();
}

void MainWindow::on_actionData_Directory_triggered()
{
	QString		DataFolderPath = QDir::toNativeSeparators( QDir( QStandardPaths::writableLocation( QStandardPaths::DataLocation ) ).absolutePath() );

	QDesktopServices::openUrl( QUrl( "file:///" + DataFolderPath ) );
}

void MainWindow::on_actionGitHub_Page_triggered()
{
	QDesktopServices::openUrl( QUrl( "https://github.com/bigfug/Fugio" ) );
}

void MainWindow::checkRecoveryFiles( void )
{
	QDir		TmpDir( QStandardPaths::writableLocation( QStandardPaths::DataLocation ) );

	QStringList	FileNameFilters;

	FileNameFilters << "fugio_tmp_*.fug";

	bool		RecoveryFound = false;
	bool		RecoveryDelete = false;

	for( const QString &FN : TmpDir.entryList( FileNameFilters, QDir::Files ) )
	{
		if( !RecoveryFound )
		{
			if( QMessageBox::question( this, tr( "Load Recovery Files?" ), tr( "Recovery files found - reload them?" ), QMessageBox::Yes, QMessageBox::No ) == QMessageBox::No )
			{
				if( QMessageBox::question( this, tr( "Delete Recovery Files?" ), tr( "Recovery files found - delete them?" ), QMessageBox::Yes, QMessageBox::No ) == QMessageBox::No )
				{
					return;
				}

				RecoveryDelete = true;
			}

			RecoveryFound = true;
		}

		if( RecoveryDelete )
		{
			QFile::remove( TmpDir.absoluteFilePath( FN ) );
		}
		else
		{
			QSharedPointer<fugio::ContextInterface>	 C = gApp->global().newContext();

			if( !C )
			{
				return;
			}

			ContextSubWindow		*SubWin = findContextWindow( C );

			if( !SubWin )
			{
				return;
			}

			SubWin->contextWidget()->loadRecovery( TmpDir.absoluteFilePath( FN ) );
		}
	}
}

void MainWindow::initBegin()
{
	ui->mNodeList->setNodelistUpdate( false );

	//mWizard.setModal( true );
	//mWizard.show();
}

void MainWindow::initEnd()
{
	ui->mNodeList->setNodelistUpdate( true );

	ui->mLogger->verticalScrollBar()->setValue( ui->mLogger->verticalScrollBar()->maximum() );
}

void MainWindow::promptUserForPatch()
{
	PatchPromptDialog		PPD( this );

	PPD.exec();
}

void MainWindow::loadPatch( const QString &pFileName )
{
	QSharedPointer<fugio::ContextInterface>	 C = gApp->global().newContext();

	if( !C )
	{
		return;
	}

	ContextSubWindow		*SubWin = findContextWindow( C );

	if( !SubWin )
	{
		return;
	}

	SubWin->contextWidget()->load( pFileName );
}

void MainWindow::on_actionCheck_for_updates_triggered()
{
	QDir	AppDir = QDir( qApp->applicationDirPath() );

#if defined( Q_OS_MAC )
	AppDir.cdUp();
	AppDir.cdUp();
	AppDir.cdUp();
#endif

	qInfo() << "Running maintenancetool from" << AppDir.absolutePath();

	QStringList	PrcArg;

	PrcArg << "--updater";

#if defined( Q_OS_MACX )
	QProcess::startDetached( "./maintenancetool.app/Contents/MacOS/maintenancetool", PrcArg, AppDir.absolutePath() );
#elif defined( Q_OS_WIN )
	QProcess::startDetached( "maintenancetool.exe", PrcArg, AppDir.absolutePath() );
#elif defined( Q_OS_LINUX )
#else
#warning No Update Process Defined
#endif
}


void MainWindow::menuAddEntry( fugio::MenuId pMenuId, QString pEntry, QObject *pObject, const char *pSlot )
{
	switch( pMenuId )
	{
		case fugio::MenuId::HELP:
			ui->menuHelp->addAction( pEntry, pObject, pSlot );
			break;
	}
}

void MainWindow::on_actionSave_all_triggered()
{
	for( QSharedPointer<fugio::ContextInterface> Context : gApp->global().contexts() )
	{
		ContextSubWindow	*SW = findContextWindow( Context );

		if( SW )
		{
			ContextWidgetPrivate	*CW = SW->contextWidget();

			if( !CW->undoStack()->isClean() )
			{
				ui->mWorkArea->setActiveSubWindow( SW );

				CW->userSave();
			}
		}
	}

}

void MainWindow::loggerContextMenu( const QPoint &pPoint )
{
	QMenu		*Menu = ui->mLogger->createStandardContextMenu();

	Menu->addAction( "Clear", ui->mLogger, SLOT(clear()) );

	Menu->exec( ui->mLogger->mapToGlobal( pPoint ) );

	delete Menu;
}

void MainWindow::on_actionOptions_triggered()
{
	SettingsDialog		Dialog( this );

	Dialog.exec();
}


QMainWindow *MainWindow::mainWindow()
{
	return( this );
}

void MainWindow::setEditTarget( fugio::EditInterface *pEditTarget )
{
	if( mEditTarget != pEditTarget )
	{
		mEditTarget = pEditTarget;

		emit mEditorSignals.editTargetChanged( mEditTarget );

		onEditTarget( mEditTarget );
	}
}

fugio::EditorSignals *MainWindow::qobject()
{
	return( &mEditorSignals );
}

const fugio::EditorSignals *MainWindow::qobject() const
{
	return( &mEditorSignals );
}

void MainWindow::menuAddFileImporter( QString pFilter, fugio::FileImportFunction pFunc )
{
	mImportFunctions.insert( pFilter, pFunc );
}

void MainWindow::on_actionImport_triggered()
{
	QSharedPointer<fugio::ContextInterface>	CI = currentContext();

	if( !CI )
	{
		return;
	}

	QStringList	Filters = mImportFunctions.keys();

	std::sort( Filters.begin(), Filters.end() );

	QString		SelectedFilter;
	QString		SelectedFile;

	SelectedFile = QFileDialog::getOpenFileName( this, tr( "Import file(s)" ), mImportDirectory, Filters.join( ";;" ), &SelectedFilter );

	if( !SelectedFile.isEmpty() )
	{
		fugio::FileImportFunction	ImportFunction = mImportFunctions.value( SelectedFilter );

		if( ImportFunction )
		{
			if( !ImportFunction( SelectedFile, CI.data() ) )
			{

			}
		}

		QFileInfo	FI( SelectedFile );

		mImportDirectory = FI.dir().absolutePath();
	}
}

QMap<fugio::SettingsInterface *, QWidget *> MainWindow::createSettings()
{
	QMap<fugio::SettingsInterface *, QWidget *>	WidLst;

	for( fugio::SettingsInterface *I : mSettingsInterfaces )
	{
		QWidget *W = I->settingsWidget();

		if( W )
		{
			WidLst.insert( I, W );
		}
	}

	return( WidLst );
}

void MainWindow::registerSettings( fugio::SettingsInterface *pSetInt )
{
	mSettingsInterfaces.removeAll( pSetInt );
	mSettingsInterfaces.append( pSetInt );
}

void MainWindow::unregisterSettings( fugio::SettingsInterface *pSetInt )
{
	mSettingsInterfaces.removeAll( pSetInt );
}

void MainWindow::on_actionSave_Patch_Image_triggered()
{
	ContextWidgetPrivate		*CV = qobject_cast<ContextWidgetPrivate *>( ui->mWorkArea->currentSubWindow()->widget() );

	if( !CV )
	{
		return;
	}

	CV->userSaveImage();
}
