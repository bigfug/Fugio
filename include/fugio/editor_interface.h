#ifndef EDITOR_INTERFACE_H
#define EDITOR_INTERFACE_H

#include <QObject>
#include <QUuid>

#include <fugio/global.h>

class QMainWindow;

FUGIO_NAMESPACE_BEGIN
class ContextInterface;
class EditInterface;
class EditorSignals;
FUGIO_NAMESPACE_END

FUGIO_NAMESPACE_BEGIN

#define IID_EDITOR		(QUuid("{ed673102-dcaa-4f38-b98e-6a7886f26a65}"))

typedef enum MenuId
{
	HELP
} MenuId;

typedef bool (*FileImportFunction)( QString pFilename, fugio::ContextInterface *pContext );

class SettingsInterface
{
public:
	virtual ~SettingsInterface( void ) {}

	virtual QWidget *settingsWidget( void ) = 0;

	virtual void settingsAccept( QWidget *pWidget ) = 0;
};

class EditorInterface
{
public:
	virtual ~EditorInterface( void ) {}

	virtual fugio::EditorSignals *qobject( void ) = 0;

	virtual const fugio::EditorSignals *qobject( void ) const = 0;

	//-------------------------------------------------------------------------
	// Main Window

	virtual QMainWindow *mainWindow( void ) = 0;

	//-------------------------------------------------------------------------

	virtual void setEditTarget( fugio::EditInterface *pEditTarget ) = 0;

	//-------------------------------------------------------------------------
	// Adding menu entries to the editor application

	virtual void menuAddEntry( fugio::MenuId, QString pName, QObject *pObject, const char *pSlot ) = 0;

	virtual void menuAddFileImporter( QString pFilter, FileImportFunction pFunc ) = 0;

	//-------------------------------------------------------------------------
	// Settings

	virtual void registerSettings( SettingsInterface *pSetInt ) = 0;
	virtual void unregisterSettings( SettingsInterface *pSetInt ) = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::SettingsInterface, "com.bigfug.fugio.editor.settings/1.0" )

Q_DECLARE_INTERFACE( fugio::EditorInterface, "com.bigfug.fugio.editor/1.0" )

#endif // EDITOR_INTERFACE_H
