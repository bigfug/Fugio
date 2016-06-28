#include "syntaxhighlighterlua.h"

#include <QDebug>

SyntaxHighlighterLua::SyntaxHighlighterLua(QObject *parent) :
	QSyntaxHighlighter(parent)
{
}

SyntaxHighlighterLua::SyntaxHighlighterLua( QTextDocument *pDocument )
	: QSyntaxHighlighter( pDocument )
{
	HighlightingRule rule;

	keywordFormat.setForeground(Qt::darkBlue);
	keywordFormat.setFontWeight(QFont::Bold);
	QStringList keywordPatterns;
	keywordPatterns << "and"
					<< "break"
					<< "do"
					<< "else"
					<< "elseif"
					<< "end"
					<< "false"
					<< "for"
					<< "function"
					<< "if"
					<< "in"
					<< "local"
					<< "nil"
					<< "not"
					<< "or"
					<< "repeat"
					<< "return"
					<< "then"
					<< "true"
					<< "until"
					<< "while";

	foreach (const QString &pattern, keywordPatterns)
	{
		rule.pattern = QRegExp( QString( "\\b%1\\b" ).arg( pattern ) );
		rule.format = keywordFormat;
		highlightingRules.append(rule);
	}

	variableFormat.setForeground(Qt::darkRed);
	variableFormat.setFontWeight(QFont::Bold);

	QStringList variablePatterns;
//	variablePatterns << "and"
//					 << "break"
//					 << "while";

	foreach (const QString &pattern, variablePatterns)
	{
		rule.pattern = QRegExp( QString( "\\b%1\\b" ).arg( pattern ) );
		rule.format = variableFormat;
		highlightingRules.append(rule);
	}

	classFormat.setFontWeight(QFont::Bold);
	classFormat.setForeground(Qt::darkMagenta);
	rule.pattern = QRegExp("\\bQ[A-Za-z]+\\b");
	rule.format = classFormat;
	highlightingRules.append(rule);

	quotationFormat.setForeground(Qt::darkGreen);
	rule.pattern = QRegExp("\".*\"");
	rule.format = quotationFormat;
	highlightingRules.append(rule);

	functionFormat.setFontItalic(true);
	functionFormat.setForeground(Qt::blue);
	rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
	rule.format = functionFormat;
	highlightingRules.append(rule);

	singleLineCommentFormat.setForeground(Qt::red);
	rule.pattern = QRegExp("--[^\n]*");
	rule.format = singleLineCommentFormat;
	highlightingRules.append(rule);

	errorFormat.setBackground( QBrush( Qt::red ) );
	errorFormat.setProperty( QTextFormat::FullWidthSelection, true );

	//	defineFormat.setForeground(Qt::darkMagenta);
	//	rule.pattern = QRegExp("#[^\n]*");
	//	rule.format = defineFormat;
	//highlightingRules.append(rule);

	multiLineCommentFormat.setForeground( Qt::red );

	commentStartExpression = QRegExp( "--\\[\\[" );
	commentEndExpression = QRegExp( "\\]\\]--" );
}

SyntaxHighlighterLua::~SyntaxHighlighterLua( void )
{
}

void SyntaxHighlighterLua::clearErrors()
{
	mErrorData.clear();

	emit errorsUpdated();
}

void SyntaxHighlighterLua::setErrors( const QString &pErrorText)
{
	QString			EL = pErrorText;

	mErrorData.clear();

	if( EL.startsWith( '"' ) )
	{
		EL.remove( 0, 1 );
	}

	if( EL.endsWith( '"' ) )
	{
		EL.chop( 1 );
	}

	QStringList		SL = EL.trimmed().split( '\n' );

	for( QString S : SL )
	{
		S = S.trimmed();

//		if( S.startsWith( "ERROR: " ) )
//		{
//			S.remove( 0, 7 );

//			QRegExp		RE( ":(\\d+):" );

//			int			LN = 0;

//			if( RE.indexIn( S ) )
//			{
//				LN = RE.cap( 1 ).toInt();
//			}

//			if( LN > 0 )
//			{
//				QStringList		SL2 = S.split( ": " );

//				SL2.removeFirst();

//				mErrorData.insert( LN, SL2.join( ": " ) );
//			}
//			else
//			{
				mErrorData.insert( 0, S );
//			}
//		}
	}

	emit errorsUpdated();
}

QStringList SyntaxHighlighterLua::errorList( int pLineNumber ) const
{
	QStringList		SL;

	if( mErrorData.contains( pLineNumber ) )
	{
		SL << mErrorData.values( pLineNumber );
	}

	return( SL );
}

void SyntaxHighlighterLua::highlightBlock( const QString &text )
{
	for( const HighlightingRule &rule : highlightingRules )
	{
		QRegExp expression( rule.pattern );

		int index = expression.indexIn(text);

		while( index >= 0 )
		{
			int length = expression.matchedLength();

			setFormat( index, length, rule.format );

			index = expression.indexIn(text, index + length);
		}
	}

	setCurrentBlockState( 0 );

	int startIndex = 0;

	if( previousBlockState() != 1 )
	{
		startIndex = commentStartExpression.indexIn(text);
	}

	while( startIndex >= 0 )
	{
		int endIndex = commentEndExpression.indexIn(text, startIndex);
		int commentLength;

		if (endIndex == -1)
		{
			setCurrentBlockState(1);

			commentLength = text.length() - startIndex;
		}
		else
		{
			commentLength = endIndex - startIndex + commentEndExpression.matchedLength();
		}

		setFormat(startIndex, commentLength, multiLineCommentFormat);

		startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
	}

	const int LineNumber = currentBlock().firstLineNumber() + 1;

	if( mErrorData.contains( LineNumber ) )
	{
		setFormat( 0, currentBlock().length(), errorFormat );
	}
}

