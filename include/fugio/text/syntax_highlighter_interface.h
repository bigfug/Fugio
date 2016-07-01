#ifndef INTERFACE_SYNTAX_HIGHLIGHTER_H
#define INTERFACE_SYNTAX_HIGHLIGHTER_H

#include <QtPlugin>
#include <QSyntaxHighlighter>

#include <fugio/global.h>

#define IID_SYNTAX_HIGHLIGHTER			(QUuid("{E8D7D957-61EB-43cd-A51E-5236622AF5A9}"))

FUGIO_NAMESPACE_BEGIN

class SyntaxHighlighterInterface
{
public:
	virtual ~SyntaxHighlighterInterface( void ) {}

	virtual QSyntaxHighlighter *highlighter( QTextDocument *pDocument ) = 0;

	virtual QStringList errorList( int pLineNumber ) const = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::SyntaxHighlighterInterface, "com.bigfug.fugio.syntax-highlighter/1.0" )

#endif // INTERFACE_SYNTAX_HIGHLIGHTER_H
