#ifndef TEXTEDITORFORM_H
#define TEXTEDITORFORM_H

#include <QWidget>
#include "codeeditor.h"
#include <fugio/text/syntax_highlighter_instance_interface.h>

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

	void setHighlighter( fugio::SyntaxHighlighterInstanceInterface *pHighlighter );

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

	void setSyntaxNone( void );

	void setSyntaxDefault( void );

	void setSyntax( const QUuid &pUuid );

private:
	Ui::TextEditorForm						*ui;
	fugio::SyntaxHighlighterInstanceInterface		*mHighlighter;
	QString									 mFileName;
};

#endif // TEXTEDITORFORM_H
