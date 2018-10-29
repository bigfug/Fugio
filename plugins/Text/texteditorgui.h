#ifndef TEXTEDITORGUI_H
#define TEXTEDITORGUI_H

#include <QObject>

#include <QPlainTextEdit>
#include <QSyntaxHighlighter>
#include <QDockWidget>

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_interface.h>

#include <fugio/core/uuid.h>

#include <fugio/nodecontrolbase.h>

#include <fugio/text/syntax_highlighter_instance_interface.h>
#include <fugio/text/syntax_error_interface.h>

class TextEditorNode;
class TextEditorForm;

class TextEditorGui : public QObject
{
	Q_OBJECT

public:
	TextEditorGui( TextEditorNode *pNode, Qt::DockWidgetArea pDockWidgetArea, bool pDockVisible )
		: mNode( pNode ), mDockWidget( Q_NULLPTR ), mTextEdit( Q_NULLPTR ),
		mHighlighter( Q_NULLPTR ), mDockArea( pDockWidgetArea ),
		mDockVisible( pDockVisible )
	{

	}

	virtual ~TextEditorGui( void ) Q_DECL_OVERRIDE {}

	Q_INVOKABLE void initialise( void );

signals:
	void textModified( QString pText );

	void textChanged( QString pText );

	void modifiedChanged( bool pModified );

private:
	void setupTextEditor( QPlainTextEdit *pTextEdit );

	bool isBuffered( void ) const;

private slots:
	void modificationUpdated( bool pModified );

	void editorUpdated( void );

	void editorUpdateClicked( void );

public slots:
	void bufferedUpdated( bool pBuffered );

	void show( void );

	void textUpdated( QString pText );

	void syntaxHighlighterUpdated( fugio::SyntaxHighlighterInstanceInterface *pSHI );

	void syntaxErrorsUpdated( QList<fugio::SyntaxError> pSyntaxErrors );

private:
	TextEditorNode		*mNode;
	QDockWidget										*mDockWidget;
	TextEditorForm									*mTextEdit;
	fugio::SyntaxHighlighterInstanceInterface		*mHighlighter;
	bool											 mBuffered;
	QString											 mCurrentText;
	Qt::DockWidgetArea								 mDockArea;
	bool											 mDockVisible;
};

#endif // TEXTEDITORGUI_H
