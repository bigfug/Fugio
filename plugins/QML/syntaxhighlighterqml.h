#ifndef SYNTAXHIGHLIGHTERQML_H
#define SYNTAXHIGHLIGHTERQML_H

#include <QSyntaxHighlighter>

#include <fugio/text/syntax_highlighter_instance_interface.h>

class SyntaxHighlighterQML : public QSyntaxHighlighter, public fugio::SyntaxHighlighterInstanceInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::SyntaxHighlighterInstanceInterface )

public:
	explicit SyntaxHighlighterQML( QObject *pParent = 0 );

	virtual ~SyntaxHighlighterQML( void );

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

	QList<fugio::SyntaxError>		 mSyntaxErrors;
};

#endif // SYNTAXHIGHLIGHTERQML_H
