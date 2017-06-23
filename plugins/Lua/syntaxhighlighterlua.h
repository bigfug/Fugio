#ifndef SYNTAXHIGHLIGHTERLUA_H
#define SYNTAXHIGHLIGHTERLUA_H

#include <fugio/text/syntax_highlighter_instance_interface.h>

#include <QSyntaxHighlighter>

class SyntaxHighlighterLua : public QSyntaxHighlighter, public fugio::SyntaxHighlighterInstanceInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::SyntaxHighlighterInstanceInterface )

public:
	explicit SyntaxHighlighterLua( QObject *pParent = nullptr );

	virtual ~SyntaxHighlighterLua( void ) {}

	// SyntaxHighlighterInstanceInterface interface
public:
	virtual QSyntaxHighlighter *highlighter() Q_DECL_OVERRIDE
	{
		return( this );
	}

	virtual void updateErrors( QList<fugio::SyntaxError> pSyntaxErrors ) Q_DECL_OVERRIDE;

protected:
	virtual void highlightBlock( const QString &pTextBlock ) Q_DECL_OVERRIDE;

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

	QList<fugio::SyntaxError>	mSyntaxErrors;
};


#endif // SYNTAXHIGHLIGHTERLUA_H
