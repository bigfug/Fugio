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
	void updateClicked();

	void textOpen();

	void textSave();

	void textSaveAs();

	void cursorPositionChanged( void );

private:
	Ui::TextEditorForm						*ui;
	fugio::SyntaxHighlighterInterface		*mHighlighter;
	QString									 mFileName;
};

#endif // TEXTEDITORFORM_H
