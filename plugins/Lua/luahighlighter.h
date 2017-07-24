#ifndef LUAHIGHLIGHTER_H
#define LUAHIGHLIGHTER_H

#include <QObject>
#include <fugio/text/syntax_highlighter_instance_interface.h>
#include "syntaxhighlighterlua.h"

class LuaHighlighter : public QObject, public fugio::SyntaxHighlighterInstanceInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::SyntaxHighlighterInstanceInterface )

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

	virtual QList<fugio::SyntaxError> errorList( void ) const Q_DECL_OVERRIDE
	{
		return( mHighlighter ? mHighlighter->errorList() : QList<fugio::SyntaxError>() );
	}

private:
	QPointer<SyntaxHighlighterLua>				mHighlighter;
};

#endif // LUAHIGHLIGHTER_H
