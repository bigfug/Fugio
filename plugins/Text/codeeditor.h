#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>
#include <fugio/global.h>

FUGIO_NAMESPACE_BEGIN
class SyntaxHighlighterInterface;
FUGIO_NAMESPACE_END

class CodeEditor : public QPlainTextEdit
{
	Q_OBJECT

public:
	explicit CodeEditor(QWidget *parent = 0);

	void lineNumberAreaPaintEvent(QPaintEvent *event);
	int lineNumberAreaWidth();

	void setHighlighter( fugio::SyntaxHighlighterInterface *pHighlighter )
	{
		mHighlighter = pHighlighter;
	}

protected:
	void resizeEvent(QResizeEvent *event);

public slots:
	void highlightCurrentLine();

private slots:
	void updateLineNumberAreaWidth(int newBlockCount);
	void updateLineNumberArea(const QRect &, int);

private:
	QWidget							*lineNumberArea;
	fugio::SyntaxHighlighterInterface		*mHighlighter;
};

#endif // CODEEDITOR_H
