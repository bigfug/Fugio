#include "luahighlighter.h"

QSyntaxHighlighter *LuaHighlighter::highlighter( QTextDocument *pDocument )
{
	if( !mHighlighter )
	{
		mHighlighter = new SyntaxHighlighterLua( pDocument );
	}

	return( mHighlighter );
}

void LuaHighlighter::clearErrors()
{
	if( mHighlighter )
	{
		mHighlighter->clearErrors();
	}
}

void LuaHighlighter::setErrors( const QString &pErrorText )
{
	if( mHighlighter )
	{
		mHighlighter->setErrors( pErrorText );
	}
}
