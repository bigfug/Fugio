#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>
#include <fugio/global.h>

#include <fugio/text/syntax_highlighter_instance_interface.h>

FUGIO_NAMESPACE_BEGIN
class SyntaxHighlighterInstanceInterface;
FUGIO_NAMESPACE_END

class CodeEditor : public QPlainTextEdit
{
	Q_OBJECT

public:
	explicit CodeEditor(QWidget *parent = 0);

	void setSyntaxErrors( QList<fugio::SyntaxError> pSyntaxErrors );

	void lineNumberAreaPaintEvent(QPaintEvent *event);
	int lineNumberAreaWidth();

protected:
	void resizeEvent(QResizeEvent *event);

public slots:
	void highlightCurrentLine();

private slots:
	void updateLineNumberAreaWidth(int newBlockCount);
	void updateLineNumberArea(const QRect &, int);

private:
	QWidget								*lineNumberArea;
	QList<fugio::SyntaxError>			 mSyntaxErrors;
};

#endif // CODEEDITOR_H
