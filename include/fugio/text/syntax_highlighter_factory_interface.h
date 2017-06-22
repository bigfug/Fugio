#ifndef SYNTAX_HIGHLIGHTER_FACTORY_INTERFACE_H
#define SYNTAX_HIGHLIGHTER_FACTORY_INTERFACE_H

#include <QtPlugin>

#include <fugio/global.h>

FUGIO_NAMESPACE_BEGIN
class SyntaxHighlighterInstanceInterface;
FUGIO_NAMESPACE_END

FUGIO_NAMESPACE_BEGIN

class SyntaxHighlighterFactoryInterface
{
public:
	virtual ~SyntaxHighlighterFactoryInterface( void ) {}

	virtual SyntaxHighlighterInstanceInterface *syntaxHighlighterInstance( void ) const = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::SyntaxHighlighterFactoryInterface, "com.bigfug.fugio.syntax-highlighter-factory/1.0" )

#endif // SYNTAX_HIGHLIGHTER_FACTORY_INTERFACE_H
