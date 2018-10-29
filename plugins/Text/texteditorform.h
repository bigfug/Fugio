#ifndef TEXTEDITORFORM_H
#define TEXTEDITORFORM_H

#include <QWidget>
#include "codeeditor.h"
#include <fugio/text/syntax_highlighter_instance_interface.h>

namespace Ui {
class TextEditorForm;
}

#include "texteditornode.h"

class TextEditorForm : public QWidget
{
	Q_OBJECT
	
public:
	explicit TextEditorForm( QWidget *pParent = Q_NULLPTR );

	virtual ~TextEditorForm( void ) Q_DECL_OVERRIDE;

	QPlainTextEdit *textEdit( void );

	void setSyntaxErrors( QList<fugio::SyntaxError> pSyntaxErrors );

signals:
	void updateText( void );

	void syntaxChanged( TextEditorNode::HighlighterType pHighlighterType, QUuid pUuid );

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
	Ui::TextEditorForm								*ui;
	QString											 mFileName;
	QList<fugio::SyntaxError>						 mSyntaxErrors;
};

#endif // TEXTEDITORFORM_H
