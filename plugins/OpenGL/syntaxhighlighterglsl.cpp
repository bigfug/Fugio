#include "syntaxhighlighterglsl.h"

#include <QDebug>
#include <QTextDocument>

SyntaxHighlighterGLSL::SyntaxHighlighterGLSL(QObject *parent) :
	QSyntaxHighlighter(parent)
{
}

SyntaxHighlighterGLSL::SyntaxHighlighterGLSL( QTextDocument *pDocument )
	: QSyntaxHighlighter( pDocument )
{
	HighlightingRule rule;

	keywordFormat.setForeground(Qt::darkBlue);
	keywordFormat.setFontWeight(QFont::Bold);
	QStringList keywordPatterns;
	keywordPatterns << "const"
					<< "break"
					<< "continue"
					<< "do"
					<< "else"
					<< "for"
					<< "if"
					<< "discard"
					<< "return"
					<< "switch"
					<< "case"
					<< "default"
					<< "subroutine"
					<< "in"
					<< "out"
					<< "inout"
					<< "uniform"
					<< "varying"
					<< "attribute"
					<< "noperspective"
					<< "flat"
					<< "smooth"
					<< "layout"
					<< "struct"
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
	variablePatterns << "bool"
					 << "float"
					 << "double"
					 << "int"
					 << "uint"
					 << "bvec2"
					 << "bvec3"
					 << "bvec4"
					 << "ivec2"
					 << "ivec3"
					 << "ivec4"
					 << "uvec2"
					 << "uvec3"
					 << "uvec4"
					 << "vec2"
					 << "vec3"
					 << "vec4"
					 << "mat2"
					 << "mat3"
					 << "mat4"
					 << "centroid"
					 << "patch"
					 << "sample"
					 << "dvec2"
					 << "dvec3"
					 << "dvec4"
					 << "dmat2"
					 << "dmat3"
					 << "dmat4"
					 << "mat2x2"
					 << "mat2x3"
					 << "mat2x4"
					 << "mat3x2"
					 << "mat3x3"
					 << "mat3x4"
					 << "mat4x2"
					 << "mat4x3"
					 << "mat4x4"
					 << "dmat2x2"
					 << "dmat2x3"
					 << "dmat2x4"
					 << "dmat3x2"
					 << "dmat3x3"
					 << "dmat3x4"
					 << "dmat4x2"
					 << "dmat4x3"
					 << "dmat4x4"
					 << "sampler1d"
					 << "sampler2d"
					 << "sampler3d"
					 << "samplercube"
					 << "sampler1dshadow"
					 << "sampler2dshadow"
					 << "samplercubeshadow"
					 << "sampler1darray"
					 << "sampler2darray"
					 << "sampler1darrayshadow"
					 << "sampler2darrayshadow"
					 << "isampler1d"
					 << "isampler2d"
					 << "isampler3d"
					 << "isamplercube"
					 << "isampler1darray"
					 << "isampler2darray"
					 << "usampler1d"
					 << "usampler2d"
					 << "usampler3d"
					 << "usamplercube"
					 << "usampler1darray"
					 << "usampler2darray"
					 << "sampler2drect"
					 << "sampler2drectshadow"
					 << "isampler2drect"
					 << "usampler2drect"
					 << "samplerbuffer"
					 << "isamplerbuffer"
					 << "usamplerbuffer"
					 << "samplercubearray"
					 << "samplercubearrayshadow"
					 << "isamplercubearray"
					 << "usamplercubearray"
					 << "sampler2dms"
					 << "isampler2dms"
					 << "usampler2dms"
					 << "sampler2dmsarray"
					 << "isampler2dmsarray"
					 << "usampler2dmsarray"
					 << "void";

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
	rule.pattern = QRegExp("//[^\n]*");
	rule.format = singleLineCommentFormat;
	highlightingRules.append(rule);

	errorFormat.setBackground( QBrush( Qt::red ) );
	errorFormat.setProperty( QTextFormat::FullWidthSelection, true );

	//	defineFormat.setForeground(Qt::darkMagenta);
	//	rule.pattern = QRegExp("#[^\n]*");
	//	rule.format = defineFormat;
	//highlightingRules.append(rule);

	multiLineCommentFormat.setForeground(Qt::red);

	commentStartExpression = QRegExp("/\\*");
	commentEndExpression = QRegExp("\\*/");
}

SyntaxHighlighterGLSL::~SyntaxHighlighterGLSL( void )
{
}

void SyntaxHighlighterGLSL::clearErrors()
{
	mErrorData.clear();

	emit errorsUpdated();

	rehighlight();
}

void SyntaxHighlighterGLSL::setErrors( const QString &pErrorText)
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

	QStringList		SL = EL.split( '\n' );

	for( QString S : SL )
	{
		S = S.trimmed();

		if( S.isEmpty() )
		{
			continue;
		}

		if( S.startsWith( "ERROR: " ) )
		{
			S.remove( 0, 7 );

			QRegExp		RE( ":(\\d+):" );

			int			LN = 0;

			if( RE.indexIn( S ) )
			{
				LN = RE.cap( 1 ).toInt();
			}

			if( LN > 0 )
			{
				QStringList		SL2 = S.split( ": " );

				SL2.removeFirst();

				mErrorData.insert( LN, SL2.join( ": " ) );
			}
			else
			{
				mErrorData.insert( 0, S );
			}
		}
		else
		{
//			QRegExp	R = QRegExp( "/\\(\\d+\\)\\s+:\\s+\\w+\\s+\\w+:\\s[\\s\\S]+/" );

//			R.indexIn( S );

//			QStringList		L = R.capturedTexts();

			mErrorData.insert( 0, S );
		}
	}

	emit errorsUpdated();

	rehighlight();
}

QStringList SyntaxHighlighterGLSL::errorList( int pLineNumber ) const
{
	QStringList		SL;

	if( mErrorData.contains( pLineNumber ) )
	{
		SL << mErrorData.values( pLineNumber );
	}

	return( SL );
}

void SyntaxHighlighterGLSL::highlightBlock( const QString &text )
{
	for( const HighlightingRule &rule : highlightingRules )
	{
		QRegExp expression(rule.pattern);

		int index = expression.indexIn(text);

		while (index >= 0)
		{
			int length = expression.matchedLength();

			setFormat(index, length, rule.format);

			index = expression.indexIn(text, index + length);
		}
	}

	setCurrentBlockState(0);

	int startIndex = 0;

	if (previousBlockState() != 1)
	{
		startIndex = commentStartExpression.indexIn(text);
	}

	while (startIndex >= 0)
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
