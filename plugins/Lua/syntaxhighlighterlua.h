#ifndef SYNTAXHIGHLIGHTERLUA_H
#define SYNTAXHIGHLIGHTERLUA_H

#include <fugio/text/syntax_highlighter_instance_interface.h>

#include <QSyntaxHighlighter>

class SyntaxHighlighterLua : public QSyntaxHighlighter
{
	Q_OBJECT

public:
	explicit SyntaxHighlighterLua( QObject *pParent = 0 );

	explicit SyntaxHighlighterLua( QTextDocument *pDocument );

	virtual ~SyntaxHighlighterLua( void );

	void clearErrors( void );

	void setErrors( const QString &pErrorText );

	QList<fugio::SyntaxError> errorList( void ) const;

signals:
	void errorsUpdated( void );

protected:
	virtual void highlightBlock( const QString &pTextBlock );

private:
	struct HighlightingRule
	{
		QRegExp pattern;
		QTextCharFormat format;
	};

	QVector<HighlightingRule> highlightingRules;

	QVector<HighlightingRule> variableRules;

	QRegExp commentStartExpression;
	QRegExp commentEndExpression;

	QTextCharFormat keywordFormat;
	QTextCharFormat variableFormat;
	QTextCharFormat classFormat;
	QTextCharFormat defineFormat;
	QTextCharFormat singleLineCommentFormat;
	QTextCharFormat multiLineCommentFormat;
	QTextCharFormat quotationFormat;
	QTextCharFormat functionFormat;
	QTextCharFormat errorFormat;

	QList<fugio::SyntaxError>		 mErrorData;
};


#endif // SYNTAXHIGHLIGHTERLUA_H
