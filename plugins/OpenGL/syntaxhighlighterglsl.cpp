#include "syntaxhighlighterglsl.h"

#include <QDebug>
#include <QTextDocument>

SyntaxHighlighterGLSL::SyntaxHighlighterGLSL(QObject *parent) :
	QSyntaxHighlighter(parent)
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
					 << "sampler1D"
					 << "sampler2D"
					 << "sampler3D"
					 << "samplerCube"
					 << "sampler1DShadow"
					 << "sampler2DShadow"
					 << "samplerCubeShadow"
					 << "sampler1DArray"
					 << "sampler2DArray"
					 << "sampler1DArrayShadow"
					 << "sampler2DArrayShadow"
					 << "isampler1D"
					 << "isampler2D"
					 << "isampler3D"
					 << "isamplerCube"
					 << "isampler1DArray"
					 << "isampler2DArray"
					 << "usampler1D"
					 << "usampler2D"
					 << "usampler3D"
					 << "usamplerCube"
					 << "usampler1DArray"
					 << "usampler2DArray"
					 << "sampler2DRect"
					 << "sampler2DRectShadow"
					 << "isampler2DRect"
					 << "usampler2DRect"
					 << "samplerBuffer"
					 << "isamplerBuffer"
					 << "usamplerBuffer"
					 << "samplerCubeArray"
					 << "samplerCubeArrayShadow"
					 << "isamplerCubeArray"
					 << "usamplerCubeArray"
					 << "sampler2DMS"
					 << "isampler2DMS"
					 << "usampler2DMS"
					 << "sampler2DMSArray"
					 << "isampler2DMSArray"
					 << "usampler2DMSArray"
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
}

void SyntaxHighlighterGLSL::updateErrors(QList<fugio::SyntaxError> pSyntaxErrors)
{
	mSyntaxErrors = pSyntaxErrors;

	rehighlight();
}
