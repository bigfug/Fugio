#ifndef LUAHIGHLIGHTER_H
#define LUAHIGHLIGHTER_H

#include <QObject>
#include <fugio/text/syntax_highlighter_interface.h>
#include "syntaxhighlighterlua.h"

class LuaHighlighter : public QObject, public fugio::SyntaxHighlighterInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::SyntaxHighlighterInterface )

public:
	LuaHighlighter( QObject *pParent = 0 )
		: QObject( pParent ), mHighlighter( 0 )
	{

	}

	void clearErrors( void );

	void setErrors( const QString &pErrorText );

	//-------------------------------------------------------------------------
	// fugio::InterfaceSyntaxHighlighter

	virtual QSyntaxHighlighter *highlighter( QTextDocument *pDocument ) Q_DECL_OVERRIDE;

	virtual QStringList errorList( int pLineNumber ) const Q_DECL_OVERRIDE
	{
		return( mHighlighter ? mHighlighter->errorList( pLineNumber ) : QStringList() );
	}

private:
	QPointer<SyntaxHighlighterLua>				mHighlighter;
};

#endif // LUAHIGHLIGHTER_H
