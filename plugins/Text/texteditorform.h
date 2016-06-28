#ifndef TEXTEDITORFORM_H
#define TEXTEDITORFORM_H

#include <QWidget>
#include "codeeditor.h"
#include <fugio/text/syntax_highlighter_interface.h>

namespace Ui {
	class TextEditorForm;
}

class TextEditorForm : public QWidget
{
	Q_OBJECT
	
public:
	explicit TextEditorForm( QWidget *parent = 0 );

	virtual ~TextEditorForm( void );
	
	QPlainTextEdit *textEdit( void );

	void setHighlighter( fugio::SyntaxHighlighterInterface *pHighlighter );

signals:
	void updateText( void );

public slots:
	void errorsUpdated( void );

	void updateNodeName( const QString &pName );

private slots:
	void on_mButtonUpdate_clicked();

	void on_mButtonLoad_clicked();

	void on_mButtonSave_clicked();

	void on_mButtonSaveAs_clicked();

	void cursorPositionChanged( void );

private:
	Ui::TextEditorForm				*ui;
	fugio::SyntaxHighlighterInterface		*mHighlighter;
};

#endif // TEXTEDITORFORM_H
