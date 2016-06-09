#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QListWidgetItem>
#include <QFileSystemModel>
#include <QUndoStack>
#include <QMdiSubWindow>
#include <QSignalMapper>

#include <fugio/edit_interface.h>

#include <fugio/node_interface.h>

#include "wizards/firsttimewizard.h"

namespace Ui {
	class MainWindow;
}

class ContextSubWindow;
struct ClassEntry;

class MainWindow : public QMainWindow
{
	Q_OBJECT
	
public:
	explicit MainWindow( QWidget *pParent = 0 );

	virtual ~MainWindow( void );

	void createDeviceMenu( void );

	ContextSubWindow *findContextWindow( QSharedPointer<fugio::ContextInterface> pContext );

signals:
	void log( const QString &pLogDat );

public slots:
	void setFPS( qreal FPS );

	void timeout( void );

	void updateWindowTitle( void );

	void checkRecoveryFiles( void );

	void initBegin( void );

	void initEnd( void );

	void promptUserForPatch( void );

	void loadPatch( const QString &pFileName );

	void addFileToRecent( const QString &pFileName );

	QString patchOpenDialog( void );

private:
	virtual void closeEvent( QCloseEvent *pEvent );

	static void logger_static( QtMsgType type, const QMessageLogContext &context, const QString &msg );

	void logger( QtMsgType type, const QMessageLogContext &context, const QString &msg );

	void updateRecentFileList( void );

	void linkWindowVisibilitySignal( QAction *pAction, QDockWidget *pWidget );

	void populateExamples( const QString &pDir, QMenu *pMenu );

	bool addExamplesPath( const QString &pPath );

private slots:
	void on_actionOpen_triggered();

	void on_actionSaveAs_triggered();

	void on_actionNew_triggered();

	void on_actionSave_triggered();

	void contextAdded( QSharedPointer<fugio::ContextInterface> pContext );
	void contextRemoved( QSharedPointer<fugio::ContextInterface> pContext );

	void on_mWorkArea_subWindowActivated( QMdiSubWindow *arg1 );

	void on_actionCursorToStart_triggered();

	void on_actionCursorToEnd_triggered();

	void on_actionPlayToggle_triggered();

	void onCleanChanged( bool pClean );

	void onDeviceMenu( void );

	void on_actionSet_duration_triggered();

	void on_actionExit_triggered();

	void on_mNodeList_itemActivated(QListWidgetItem *item);

	void on_actionAdd_Note_triggered();

	void on_actionAbout_Fugio_triggered();

	void on_actionAbout_Qt_triggered();

	void on_actionOnline_Documentation_triggered();

	void on_actionSet_Colour_triggered();

	void on_actionCut_triggered();

	void on_actionCopy_triggered();

	void on_actionPaste_triggered();

	void recentFileActivated( void );

	void buildWindowMenu( void );

	void setActiveWindow( QWidget *pWidget );

	void onExample( void );

	void onEditTarget( fugio::EditInterface *pEditTarget );

	void on_actionContact_Developers_triggered();

	void on_actionFacebook_Page_triggered();

	void fugioUrl( const QUrl &pUrl );

	void on_actionAppend_triggered();

	void on_actionSelect_to_snippit_triggered();

	void on_actionGroup_triggered();

	void on_actionGroup_Parent_triggered();

	void on_actionUngroup_triggered();

	void on_actionData_Directory_triggered();

	void on_actionGitHub_Page_triggered();

	void on_actionCheck_for_updates_triggered();

private:
	Ui::MainWindow								*ui;

	QFileSystemModel							 mFileSystem;

	QtMessageHandler							 mMessageHandler;

	QAction										*mActionUndo;
	QAction										*mActionRedo;

	QUndoStack									 mUndoStack;

	QSignalMapper								 mActiveWindowMapper;

	//FirstTimeWizard								 mWizard;
};

#endif // MAINWINDOW_H
