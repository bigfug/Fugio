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

#include <fugio/editor_interface.h>
#include <fugio/editor_signals.h>

#include "wizards/firsttimewizard.h"

namespace Ui {
	class MainWindow;
}

class ContextSubWindow;
struct ClassEntry;

class MainWindow : public QMainWindow, public fugio::EditorInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::EditorInterface )
	
public:
	explicit MainWindow( QWidget *pParent = 0 );

	virtual ~MainWindow( void );

	void createDeviceMenu( void );

	ContextSubWindow *findContextWindow( QSharedPointer<fugio::ContextInterface> pContext );


	// EditorInterface interface
protected:
	virtual QMainWindow *mainWindow() Q_DECL_OVERRIDE;
	virtual void setEditTarget(fugio::EditInterface *pEditTarget) Q_DECL_OVERRIDE;
	virtual void menuAddEntry( fugio::MenuId pMenuId, QString pEntry, QObject *pObject, const char *pSlot ) Q_DECL_OVERRIDE;
	virtual fugio::EditorSignals *qobject() Q_DECL_OVERRIDE;
	virtual const fugio::EditorSignals *qobject() const Q_DECL_OVERRIDE;
	virtual void menuAddFileImporter( QString pFilter, fugio::FileImportFunction pFunc ) Q_DECL_OVERRIDE;

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

	QStringList patchOpenDialog( void );

private:
	virtual void closeEvent( QCloseEvent *pEvent ) Q_DECL_OVERRIDE;

	static void logger_static( QtMsgType type, const QMessageLogContext &context, const QString &msg );

	void logger( QtMsgType type, const QMessageLogContext &context, const QString &msg );

	void updateRecentFileList( void );

	void linkWindowVisibilitySignal( QAction *pAction, QDockWidget *pWidget );

	void populateExamples( const QString &pDir, QMenu *pMenu );

	bool addExamplesPath( const QString &pPath );

	QSharedPointer<fugio::ContextInterface> currentContext( void );

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

	void on_actionFacebook_Users_Group_triggered();

	void on_actionSave_all_triggered();

	void loggerContextMenu( const QPoint &pPoint );

	void on_actionOptions_triggered();

	void on_actionImport_triggered();

private:
	Ui::MainWindow								*ui;

	QFileSystemModel							 mFileSystem;

	QtMessageHandler							 mMessageHandler;

	QAction										*mActionUndo;
	QAction										*mActionRedo;

	QUndoStack									 mUndoStack;

	QSignalMapper								 mActiveWindowMapper;

	fugio::EditInterface						*mEditTarget;

	fugio::EditorSignals						 mEditorSignals;

	QMap<QString,fugio::FileImportFunction>		 mImportFunctions;

	QString										 mImportDirectory;
};

#endif // MAINWINDOW_H
